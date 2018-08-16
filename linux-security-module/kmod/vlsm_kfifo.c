#include <linux/kfifo.h>
#include <linux/list.h>
#include <linux/string.h>

#include <linux/kthread.h>
#include "vlsm_kfifo.h"
#include "vlsm_utils.h"

static struct task_struct *kthr_cons = NULL;
static struct task_struct *kthr_prod = NULL;

#if 0
/* kfifo buffers */
static DECLARE_KFIFO_PTR(kfbs[kfifo_nums], kfifo_element);
/* typedef kfifo pointer */
#define kfbs_pos STRUCT_KFIFO_PTR(kfifo_element) *

#else
struct __kfifo__ __STRUCT_KFIFO_PTR(kfifo_element, 0, void);

static struct __kfifo__ kfbs[kfifo_nums];
#define kfbs_pos struct __kfifo__ *
#endif


/* 错误处理函数 */
static int error_handler(int errno)
{
    switch (errno) {
    case EAGAIN: {
        /* 发送缓存满了，需要等待处理 */
        log_error("consumer handle EAGAIN");
    } break;
    case ECONNREFUSED: {
        log_error("consumer handle ECONNREFUSED");
        /* 连接不上就不发了，等用户主动发消息过来再建立消费 */
/*        if (kthr_cons) {
            kthread_stop(kthr_cons);
            kthr_cons = NULL;
        }
*/
    } break;
    default :{
        log_error("consumer unhandled error %d", errno);
        return -1;
        /* 其他错误 */
    }
    }
    return 0;
}
/* 消费者线程 */
static int consumer_kthread(void *args)
{
    kfifo_element e;
    kfbs_pos pos;
    kfifo_callback consumer = (kfifo_callback)args;
    int ret = 0;

    if (NULL == consumer) 
        return log_error("consumer is nullptr!"), -1;

    do {
        kfbs_for_each(pos) {    /* 遍历 kfifo 队列 */

            while (kfifo_get(pos, &e)) { /* 获取数据 */
                ret = consumer((void *)&e); /* 消费数据 */
                if (ret < 0) {
                    kfifo_put(pos, &e);
                    break;
                }
            }
            if (ret < 0)
                if (!error_handler(-ret)) break;
        }
        set_current_state(TASK_UNINTERRUPTIBLE);
        /* 后面可以继续优化为事件中断驱动 */
//        wait_event_interruptible();
        schedule_timeout(5*HZ);
        set_current_state(TASK_RUNNING);

    } while (!kthread_should_stop());
    log_debug("consumer kthread break out!");
    kthr_cons = NULL;
    return 0;
}

/* 生产者线程 测试 */
static int producer_kthread(void *args)
{
    kfifo_element e;
    kfbs_pos pos;
    while (!kthread_should_stop()) {
        kfbs_for_each(pos) {
            e.type = (int)pos;
            e.size = 0;
            kfifo_put(pos, &e);
        }
        set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(3*HZ);
        set_current_state(TASK_RUNNING);
    }
    log_debug("producer kthread break out!");
    kthr_prod = NULL;
    return 0;
}

/* 默认消费者 */
static int kfifo_default_consumer(kfifo_element *e)
{
    log_debug("msg( size = %d )", e ? e->size : 0);
    return 0;
}

/* 生产者 */
int kfifo_producer(kfifo_element *e)
{
    int hash = e->type & kfifo_mask;
    log_debug("kfbs[%d] size = %d ", hash, kfifo_size(&kfbs[hash]) - kfifo_len(&kfbs[hash]));
    return e ? ~kfifo_put(&kfbs[hash], e) : -1;
}

/* 重新设定消费者线程 */
int kfifo_consumer_reset(kfifo_callback consumer)
{
    if (kthr_cons) {
        kthread_stop(kthr_cons);
        kthr_cons = NULL;
    }
    /* 启动消费者线程 */
    kthr_cons = kthread_run(consumer_kthread, consumer, "kfifo consumer thread");
    if (IS_ERR(kthr_cons))  {
        kthr_cons = NULL;
        log_error("consumer run error.");
        return -1;
    }
    return 0;
}


/* 构造 kfifo */
__init int vlsm_kfifo_init(void)
{
    /* 申请队列内存 */
    kfbs_pos pos;
    kfbs_for_each(pos) {
        if (kfifo_alloc(pos, kfifo_buffer_size, GFP_KERNEL)) {
            log_error("kfifo_alloc failed!");
        }
    }
    /* 启动消费者线程 */
    kthr_cons = kthread_run(consumer_kthread, kfifo_default_consumer, "kfifo test thread");
    if (IS_ERR(kthr_cons))  {
        kthr_cons = NULL;
        log_error("consumer run error.");
        return -1;
    }
    
    kthr_prod = kthread_run(producer_kthread, NULL, "kfifo test producer");
    if (IS_ERR(kthr_prod))  {
        kthr_prod = NULL;
        log_error("producer run error");
        return -1;
    }

    log_debug("kfifo constructor!");
    return 0;
}
/* 析构kfifo模块 */
__exit void vlsm_kfifo_exit(void)
{
    kfbs_pos pos;
    if (kthr_cons) {
        kthread_stop(kthr_cons);
        kthr_cons = NULL;
    }
    if (kthr_prod) {
        kthread_stop(kthr_prod);
        kthr_prod = NULL;
    }
    /* 释放队列内存 */
    kfbs_for_each(pos) {
        kfifo_free(pos);
    }
    log_debug("kfifo destructor");
}

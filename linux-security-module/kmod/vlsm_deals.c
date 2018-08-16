#include <linux/string.h>
#include <linux/slab.h>

#include "vlsm_deals.h"
#include "vlsm_utils.h"
#include "vlsm_nlmsg.h"

/* ------- 针对之前老接口进行适配 ------- */

/*
  自己构造消费者并通过
  kfifo_consumer_reset 函数重置消费者
  默认消费者是打印消息
  目前通过netlink单播出去
  之后，可以采用组播，共享内存等
  消费者决定，消息怎么到用户空间的方式
 */
int vlsm_kfifo_consumer(kfifo_element *e)
{
    int hs = 0, ds = 0;
    void *h = NULL, *d = NULL;

    if (!e) {
        log_error("data is empty!");
    }
    /* 规整发送数据 */
    h = (void *)e;
    hs = sizeof(struct vrv_audit_msg_elm);
    d = e->data;
    ds = e->size;

    return vlsm_nlmsg_sender(h, hs, d, ds);
}
/*
  发送数据缓冲队列中交给队列处理
  接口兼容之前的名字

  返回值: 0 成功 -1 失败
*/
int vrv_lsm_kput_msg(struct vrv_audit_msg_elm_k *v)
{
    return v ? kfifo_producer((kfifo_element *)v) : -1;
}



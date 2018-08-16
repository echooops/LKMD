#ifndef __VLSM_KFIFO_H
#define __VLSM_KFIFO_H


/*
  设计思路主要还是依据生产者消费者模式
  vlsm_hooks 函数截获数据后发送到相应的kfifo中
*/


/* 定义 kfifo 相关常量 */
/*
  目前根据type信息离散的分到不同环中
  后续调优可以根据时间类型分到不同环中
 */
#define kfifo_mask 0x7;         /* 二进制 111 */
#define kfifo_nums 8            /* kfifo 数量 */
#define kfifo_buffer_size 8196  /* kfifo 缓冲大小 */

/* kfifo 每个格子中的元素定义 */
//typedef vrv_audit_msg_elm_k kfifo_element ;
typedef struct {
    int   type;                 /* 环类型 */
    int   size;
    void* data;                 /* 数据 */
} kfifo_element;


typedef int (*kfifo_callback)(kfifo_element *);

/* 定义这组kfifo的遍历方式 */
#define kfbs_for_each(pos)                                  \
    for (pos = &kfbs[0]; pos != &kfbs[kfifo_nums]; pos++)


/* ------- Interface ------- */
extern __init int vlsm_kfifo_init(void);
extern __exit void vlsm_kfifo_exit(void);

/* 重新设定消费者线程 */
extern int kfifo_consumer_reset(kfifo_callback consumer);

/* 生产者 */
extern int kfifo_producer(kfifo_element *e);


#endif /* __VLSM_KFIFO */

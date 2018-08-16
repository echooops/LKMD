#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/mount.h>
#include <linux/cred.h>
#include <linux/errno.h>

#include "vlsm_utils.h"
#include "vlsm_nlmsg.h"
#include "vlsm_kfifo.h"
#include "vlsm_deals.h"

/* 授权方式 */
MODULE_LICENSE("GPL");
/* 作者 */
MODULE_AUTHOR("ECHO");

/* kfifo 生产者在消费者定义在 utils.c 中 */
/* 构造 */
static __init int vlsm_init(void)
{
    if (vlsm_nlmsg_init()) {
        log_error("vlsm_nlmsg_init failed!");
    }
    if (vlsm_kfifo_init()) {
        log_error("vlsm_kfifo_init failed!");
    }
    /* 注册消费者 */
    kfifo_consumer_reset(vlsm_kfifo_consumer);

    return 0;
}

/* 析构 */
static __exit void vlsm_exit(void)
{
    vlsm_kfifo_exit();

    vlsm_nlmsg_exit();
}

module_init(vlsm_init);
module_exit(vlsm_exit);

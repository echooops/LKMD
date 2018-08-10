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


#include "vlsm_nlmsg.h"

/* 授权方式 */
MODULE_LICENSE("GPL");
/* 作者 */
MODULE_AUTHOR("ECHO");


/* 构造 */
static __init int vlsm_init(void)
{
    vlsm_nlmsg_init();
    return 0;
}
/* 析构 */
static __exit void vlsm_exit(void)
{
    vlsm_nlmsg_exit();
}

module_init(vlsm_init);
module_exit(vlsm_exit);

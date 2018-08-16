#ifndef __VLSM_NLMSG_H
#define __VLSM_NLMSG_H

#include <linux/module.h>

#define NETLINK_VLSM 25  //User defined group, consistent in both kernel prog and user prog

/* ------- Interface ------- */

extern __init int vlsm_nlmsg_init(void);
extern __exit void vlsm_nlmsg_exit(void);

/* 为了不滥用kmalloc针对之前的接口而改造的sender --- 完全为了兼容之前而写的接口*/
extern int vlsm_nlmsg_sender(void *h, int hs, void *d, int ds);

#endif  /* __VLSM_NLMSG_H */

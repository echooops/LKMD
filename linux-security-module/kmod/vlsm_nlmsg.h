#ifndef __VLSM_NLMSG_H
#define __VLSM_NLMSG_H

#include <linux/module.h>

#define NETLINK_VLSM 25  //User defined group, consistent in both kernel prog and user prog

__init int vlsm_nlmsg_init(void);
__exit void vlsm_nlmsg_exit(void);

#endif  /* __VLSM_NLMSG_H */

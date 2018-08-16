#ifndef __VLSM_DAELS_H
#define __VLSM_DAELS_H

#include "vlsm_kfifo.h"
#include "vlsm_nlmsg.h"

/*接收到配置信息的回应,留做备用,返回配置应用结果等*/
#define VLSM_CFG_ECHO_FIX_MSG "VRV_CFG_XGS_HEHE_DA"

/*kernel -----> user*/
typedef enum VRV_LSM_AMSG_TYPE_T {
    VMSG_AMSG_MIN,

    VMSG_FS_MIN,
    VMSG_PATH_MKNOD,
    VMSG_PATH_UNLINK,
    VMSG_PATH_MOVE,
    VMSG_PATH_RENAME,
    VMSG_PATH_CHMOD,
    VMSG_PATH_CHOWN,
    VMSG_FILE_OPEN,
    VMSG_FILE_MODIFY,
    VMSG_FILE_COPY,
    VMSG_FS_MAX,

    VMSG_TASK_MIN,
    VMSG_TASK_CREATE,
    VMSG_TASK_FREE,
    VMSG_PROCESS_EXEC,
    VMSG_TASK_MAX,

    VMSG_SB_MIN,
    VMSG_SB_MOUNT,
    VMSG_SB_UMOUNT,
    VMSG_SB_MAX,

    VMSG_SOCK_MIN,
    VMSG_SOCK_RECV,
    VMSG_SOCK_SEND,
    VMSG_SOCK_FLOW,
    VMSG_SOCK_MAX,

    VMSG_HTTP_INFO,
    VMSG_CONN_INFO_TCP,
    VMSG_CONN_INFO_UDP,
    VMSG_ARP_INFO,
    VMSG_PORT_SCAN,
    VMSG_AMSG_MAX
} VRV_LSM_AMSG_TYPE;

/* 通信协议相关，为了兼容不做改造 */
struct vrv_audit_msg_elm {
    VRV_LSM_AMSG_TYPE msg_type;
    char ctx[0];
};

/* for kfifo */
struct vrv_audit_msg_elm_k {
    VRV_LSM_AMSG_TYPE msg_type;
    int ctx_len;
    char *ctx;
};

/* ------- Interface ------- */

/*
  消费者回调
*/
extern int vlsm_kfifo_consumer(kfifo_element *e);
/*
  保持之前的接口，发送消息到缓冲环中
*/
extern int vrv_lsm_kput_msg(struct vrv_audit_msg_elm_k *v);

#endif /* __VLSM_DAELS */

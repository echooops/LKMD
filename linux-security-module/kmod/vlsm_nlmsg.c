#include <linux/kernel.h>
#include <linux/init.h>
#include <net/sock.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/kthread.h>

#include "vlsm_nlmsg.h"
#include "vlsm_utils.h"
#include "vlsm_deals.h"
/* sock */
static struct sock *nlfd = NULL;

/* 读写锁 */
static struct {
    __u32 pid;
    rwlock_t lock;
} user_proc;


/* 为了不滥用kmalloc针对之前的接口而改造的sender --- 完全为了兼容之前而写的接口*/
int vlsm_nlmsg_sender(void *h, int hs, void *d, int ds)
{
    struct sk_buff *nl_skb = NULL;
    struct nlmsghdr *nlh = NULL;
    int ret = 0, len = 0;

    len = hs + ds;
    /* 创建skb */
    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if(!nl_skb) {
        log_error("netlink_alloc_skb error");
        return -1;
    }
    /* 看了下内核的代码，下面的函数不是直观上的put到什么列表里，而是针对nlh的做些初始化工作 */
    nlh = nlmsg_put(nl_skb, 0, 0, 0, len, 0);
    if(!nlh) {
        log_error("nlmsg_put error\n");
        nlmsg_free(nl_skb);
        return -1;
    }
    /* 拷贝头 */
    memcpy(nlmsg_data(nlh), h, hs);
    /* 拷贝数据 */
    memcpy(nlmsg_data(nlh) + hs, d, ds);

    /* 发送数据 */
    read_lock_bh(&user_proc.lock);
    if (user_proc.pid != 0) {   /* pid = 0 是发给自己 */
        ret = netlink_unicast(nlfd, nl_skb, user_proc.pid, MSG_DONTWAIT);
    }
    read_unlock_bh(&user_proc.lock);

    ret < 0 ? log_error("netlink_unicast, return: %d.", ret) : \
        log_debug("netlink_unicast, return: %d.", ret);
    return ret;
}

/* 对外普适单播发送数据接口 */
int send_to_user(char *pbuf, uint16_t len)
{
    struct sk_buff *nl_skb = NULL;
    struct nlmsghdr *nlh = NULL;
    int ret = 0;

    log_debug("send_to_user begin.");
    /* 创建skb */
    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if(!nl_skb) {
        log_error("netlink_alloc_skb error");
        return -1;
    }
    /* 看了下内核的代码，下面的函数不是直观上的put到什么列表里，而是针对nlh的做些初始化工作 */
    nlh = nlmsg_put(nl_skb, 0, 0, 0, len, 0);
    if(!nlh) {
        log_error("nlmsg_put error\n");
        nlmsg_free(nl_skb);
        return -1;
    }
    memcpy(nlmsg_data(nlh), pbuf, len);

    /* 发送数据 */
    read_lock_bh(&user_proc.lock);
    if (user_proc.pid != 0) {   /* pid = 0 是发给自己 */
        ret = netlink_unicast(nlfd, nl_skb, user_proc.pid, MSG_DONTWAIT);
    }
    read_unlock_bh(&user_proc.lock);
    //else read_unlock_bh(&user_proc.lock);

    ret < 0 ? log_error("netlink_unicast, return: %d.", ret) :  \
        log_debug("netlink_unicast, return: %d.", ret);

    return ret;
}

/* 回调函数，内核态创建netlink时传入，用于接受用户态消息 */
static void recv_callback(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    void *data = NULL;
    log_debug("skb->len:%u\n", skb->len);
    if(skb->len >= nlmsg_total_size(0)) {
        /* 取包头 */
        nlh = nlmsg_hdr(skb);
        /* 设置接收用户pid */
        write_lock_bh(&user_proc.lock);
        user_proc.pid = nlh->nlmsg_pid;
        write_unlock_bh(&user_proc.lock);
        /* 取数据 */
        data = nlmsg_data(nlh);
        if(data) {
            /* 刷配置 */
            //vlsm_update_config(data);
            /* 回消息 */
            send_to_user(VLSM_CFG_ECHO_FIX_MSG, strlen(VLSM_CFG_ECHO_FIX_MSG));
        }
    }
}

/* 构造nlmsg模块 */
__init int vlsm_nlmsg_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = recv_callback,
    };

    rwlock_init(&user_proc.lock);

    nlfd = netlink_kernel_create(&init_net, NETLINK_VLSM, &cfg);
    if(!nlfd) {
        log_error("can not create a netlink socket!\n");
        return -1;
    }
    log_debug("nlmsg constructor!");
    return 0;
}

/* 析构nlmsg模块 */
__exit void vlsm_nlmsg_exit(void)
{
    if (nlfd) {
        netlink_kernel_release(nlfd);
        nlfd = NULL;
    }
    log_debug("nlmsg destructor");
}

/*
  一些注释

  单播netlink_unicast()
  int netlink_unicast(struct sock *ssk, struct sk_buff *skb, __u32 portid, int nonblock);

  ssk:    sock结构体指针
  skb:    skb存放消息，它的data字段指向要发送的 netlink消息结构，而skb的控制块保存了消息的地址信息，前面的宏NETLINK_CB(skb)就用于方便设置该控制块
  portid: 端口id
  nonblock:表示该函数是否为非阻塞，如果为1，该函数将在没有接收缓存可利用时立即返回，而如果为0，该函数在没有接收缓存可利 用时睡眠
  返回: 发送数据的长度

  多播netlink_broadcast()
  int netlink_broadcast(struct sock *ssk, struct sk_buff *skb, __u32 portid, __u32 group, gfp_t allocation);

  ssk:    sock结构体指针
  skb:    skb存放消息，它的data字段指向要发送的 netlink消息结构，而skb的控制块保存了消息的地址信息，前面的宏NETLINK_CB(skb)就用于方便设置该控制块
  portid: 端口id
  group:  netlink组
  allocation: 内核内存分配类型，一般地为GFP_ATOMIC或 GFP_KERNEL，GFP_ATOMIC用于原子的上下文（即不可以睡眠），而GFP_KERNEL用于非原子上下文
  返回: 发送数据的长度
*/

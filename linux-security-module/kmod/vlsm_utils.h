#ifndef __VLSM_UTILS_H
#define __VLSM_UTILS_H
/**
 * CSI(Control Sequence Introducer/Initiator) sign
 * more information on https://en.wikipedia.org/wiki/ANSI_escape_code
 */
#define CSI_START                      "\033["
#define CSI_END                        "\033[0m"
/* output log front color */
#define F_BLACK                        "30;"
#define F_RED                          "31;"
#define F_GREEN                        "32;"
#define F_YELLOW                       "33;"
#define F_BLUE                         "34;"
#define F_MAGENTA                      "35;"
#define F_CYAN                         "36;"
#define F_WHITE                        "37;"
/* output log background color */
#define B_NULL
#define B_BLACK                        "40;"
#define B_RED                          "41;"
#define B_GREEN                        "42;"
#define B_YELLOW                       "43;"
#define B_BLUE                         "44;"
#define B_MAGENTA                      "45;"
#define B_CYAN                         "46;"
#define B_WHITE                        "47;"
/* output log fonts style */
#define S_BOLD                         "1m"
#define S_UNDERLINE                    "4m"
#define S_BLINK                        "5m"
#define S_NORMAL                       "22m"
/* output log default color definition: [front color] + [background color] + [show style] */
#ifndef LOG_COLOR_ASSERT
#define LOG_COLOR_ASSERT               (F_MAGENTA B_NULL S_NORMAL)
#endif
#ifndef LOG_COLOR_ERROR
#define LOG_COLOR_ERROR                (F_RED B_NULL S_NORMAL)
#endif
#ifndef LOG_COLOR_WARN
#define LOG_COLOR_WARN                 (F_YELLOW B_NULL S_NORMAL)
#endif
#ifndef LOG_COLOR_INFO
#define LOG_COLOR_INFO                 (F_CYAN B_NULL S_NORMAL)
#endif
#ifndef LOG_COLOR_DEBUG
#define LOG_COLOR_DEBUG                (F_GREEN B_NULL S_NORMAL)
#endif
#ifndef LOG_COLOR_VERBOSE
#define LOG_COLOR_VERBOSE              (F_BLUE B_NULL S_NORMAL)
#endif

/* 调试信息 */
#ifdef DEBUG
#define log_debug(fmt, args...)                                         \
    printk(KERN_INFO "[DBG] (%s) (%d): " fmt "\n", __func__, __LINE__, ##args)
#else
#define log_debug(fmt, args...)
#endif
/* 错误信息 */
#define log_error(fmt, args...)                                         \
    printk(KERN_ERR "[ERR] (%s) (%d): " fmt "\n", __func__, __LINE__, ##args)
#endif  /* __VLSM_UTILS_H */

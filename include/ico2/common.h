#ifndef ICO2_COMMON_H_
#define ICO2_COMMON_H_


/* Tasks parameters */
#define SCHEDULER_TASK_STACK_SIZE     (0x1000)
#define SCHEDULER_TASK_PRIORITY         (0x0F)

#define IDLE_TASK_STACK_SIZE          (0x2000)
#define IDLE_TASK_PRIORITY              (0x1B)

#define CDVD_TASK_STACK_SIZE         (0x1B000)
#define CDVD_TASK_PRIORITY              (0x1C)

#define CDVDST_TASK_STACK_SIZE        (0x4000)
#define CDVDST_TASK_PRIORITY            (0x1B)

#define STMGR_TASK_STACK_SIZE         (0x2000)
#define STMGR_TASK_PRIORITY             (0x1B)

#define MCMGR_TASK_STACK_SIZE         (0x2000)
#define MCMGR_TASK_PRIORITY             (0x1B)

#define JIMAKU_TASK_STACK_SIZE        (0x2000)
#define JIMAKU_TASK_PRIORITY            (0x1B)

#define SNDMGR_TASK_STACK_SIZE        (0x2000)
#define SNDMGR_TASK_PRIORITY            (0x10)

#define MAIN_TASK_STACK_SIZE          (0x6000)
#define MAIN_TASK_PRIORITY              (0x1B)

#define IOS_TASK_STACK_SIZE           (0x2000)
#define IOS_TASK_PRIORITY               (0x1D)

#define SYS_MAIN_TASK_PRIORITY          (0x0E)

/* debug */
void __assert (char const *file[], int line, char const *cond[]);
void debug_assert(char const * file[], int line);
void debug_StdPrintfDummy(const char *, ...);

#define CRASH(what, file, line, msg...) \
    debug_StdPrintfDummy(msg);          \
    debug_assert(file, line);           \
    __assert(file, line, what);

#endif // ICO2_COMMON_H_

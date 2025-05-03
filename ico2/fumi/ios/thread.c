#include "common.h"

#include <ico2/common.h>
#include <ico2/fumi.h>

/* Defines */
#define IOS_DESTROY_MGR_MESSAGES_MAX    (     2u)
#define IOS_THREAD_MESSAGES_MAX         (     8u)
#define IOS_STACK_MARK_SIZE             (    16u)
#define IOS_DESTROY_MGR_THREAD_PRIORITY (    13u)
#define IOS_DESTROY_MGR_STACK_SIZE      (IOS_TASK_STACK_SIZE)

/* Types */
typedef u32 IosMessage;
struct ThreadMsgData
{
    struct IosMsgQueue  queue;                              // 48
    int                 storage[IOS_THREAD_MESSAGES_MAX];   // 32
}; // 80 bytes

/* Statics */
static u32                              iosThreadDestroyMgrMsgBuf[IOS_DESTROY_MGR_MESSAGES_MAX];
static struct IosThreadInfo            *thread_table[MAX_THREADS];
static struct IosMsgQueue               iosThreadDestroyMgrMsgQ;
static struct IosThreadInfo             iosThreadDestroyMgrThread;
static u8                               iosThreadDestroyMgrStack[IOS_DESTROY_MGR_STACK_SIZE];

/* Data */
extern u32                              n_thread;// = 0;
extern const char const                 __assert_cond_0[];// = "0";

/* Constants (strings) */
char const * const stack_mark_begin = "<THREAD_SP>....";
char const * const stack_mark_end   = "<THREAD_SP_END>";

char const * const D_00551DD0 = "thr:thread table over flow\n";
char const * const D_00551DF0 = "ios/thread.c";
char const * const D_00551E00 = "thr:can't create thread\n";
char const * const D_00551E20 = "n_thread %d\n";
char const * const D_00551E30 = "thr:can't create stack\n";
char const * const D_00551E48 = "iosThreadDestroyMgr() in\n";
char const * const D_00551E68 = "1:n_thread %d\n";
char const * const D_00551E78 = "thr:id out of range\n";

void iosThreadDestroyMgr(void *args);

//-----------------------------------------------------------------------------
static inline int iosThreadCreateCommon(struct IosThreadInfo *th_info, void *args)
{
    int const tid = CreateThread(&th_info->param);

    th_info->threadId = tid;
    th_info->lowPrio  = 0;
    th_info->args     = args;
    
    if (tid >= MAX_THREADS) {
        CRASH(__assert_cond_0, "ios/thread.c", 0x8d, "thr:thread table over flow\n");
    } else {
        if (tid < 1) {
            CRASH(__assert_cond_0, "ios/thread.c", 0x91, "thr:can't create thread\n");
        }
        else {
            thread_table[tid] = th_info;
        }
    }

    n_thread = n_thread + 1;
    debug_StdPrintfDummy("n_thread %d\n", n_thread);
    return tid;
}

//-----------------------------------------------------------------------------
void iosThreadMain(void *args)
{
    int const tid = GetThreadId();
    struct IosThreadInfo *th_info = thread_table[tid];
    th_info->entry(args);

    if (th_info->lowPrio == 0) {
        iosThreadSetPri(th_info, IOS_COMMON_TASK_PIORITY_HI);
    }
    else {
        iosThreadSetPri(th_info, IOS_COMMON_TASK_PIORITY_LO);
    }
}

//-----------------------------------------------------------------------------
void iosThreadCreateS(
        struct IosThreadInfo *th_info,
        int unk,
        iosThreadFuncPtr entry,
        void *args,
        void *partition,
        long stack_size,
        int priority)
{
    u8 *stackPtr;
    u8 *stackPtrEnd;

    void *mem = iosMallocDebug(partition, stack_size, "ios/thread.c", 0xad);
    if (mem == NULL)
    {
        debug_StdPrintfDummy("thr:can't create stack\n");
        return;
    }

    stackPtr    = mem;
    stackPtrEnd = stackPtr + stack_size;
    
    th_info->entry          = entry;
    th_info->param.entry    = iosThreadMain;
    th_info->param.stack    = stackPtr;
 
    memcpy(stackPtr, stack_mark_begin, IOS_STACK_MARK_SIZE);
    memcpy(stackPtrEnd - IOS_STACK_MARK_SIZE, stack_mark_end, IOS_STACK_MARK_SIZE);

    th_info->param.stackSize       = stack_size - IOS_STACK_MARK_SIZE;
    th_info->param.initPriority    = priority;
    th_info->param.currentPriority = priority;
    th_info->param.gpReg           = &_gp;

    iosThreadCreateCommon(th_info, args);

    th_info->hasMsgQ  = 0;
    th_info->flags   |= IOS_THRF_ALLOCATED_STACK;
}

//-----------------------------------------------------------------------------
void iosThreadStart(struct IosThreadInfo const *th_info)
{
    StartThread(th_info->threadId, th_info->args);
}

//-----------------------------------------------------------------------------
void iosThreadStop(struct IosThreadInfo const *th_info)
{
    if (th_info == NULL) {
        ExitThread();
        return;
    }

    TerminateThread(th_info->threadId);
}

//-----------------------------------------------------------------------------
void iosThreadSleep()
{
    SleepThread();
}

//-----------------------------------------------------------------------------
void iosThreadDestroy(struct IosThreadInfo *th_info)
{
    if (th_info == NULL) {
        th_info = thread_table[GetThreadId()];
    }

    iosMsgSend(&iosThreadDestroyMgrMsgQ, th_info, 0);
}

//-----------------------------------------------------------------------------
void iosThreadSetPri( struct IosThreadInfo *th_info, int priority)
{
    if (th_info == NULL) {
        th_info = thread_table[GetThreadId()];
    }
    th_info->param.currentPriority = priority;
    ChangeThreadPriority(th_info->threadId, priority);
}

//-----------------------------------------------------------------------------
void iosThreadMessage(IosMessage *msg)
{
    struct ThreadMsgData *heapPtr;

    int msgResult;
    struct IosThreadInfo *th_info = thread_table[GetThreadId()];
    
    if (th_info->hasMsgQ == 0) {
        th_info->hasMsgQ = 1;
        heapPtr = (struct ThreadMsgData*)iosMallocDebug(
            ios_partition_root, sizeof(struct ThreadMsgData), "ios/thread.c", 0x1de);
            th_info->msgQ = &heapPtr->queue;
            iosMsgQueueCreate(&heapPtr->queue, &heapPtr->storage, IOS_THREAD_MESSAGES_MAX);
        }
        
        msgResult = iosMsgSend(th_info->msgQ, msg, 0);
        debug_StdPrintfDummy("th:msg %d\n", msgResult);
    }
    
//-----------------------------------------------------------------------------
void iosThreadName(struct IosThreadInfo *info, char const *name)
{
    strcpy(info->name, name);
}

//-----------------------------------------------------------------------------
void iosThreadSuspend(struct IosThreadInfo const *th_info)
{
    SuspendThread(th_info->threadId);
}

//-----------------------------------------------------------------------------
void iosThreadResume(struct IosThreadInfo const *th_info)
{
    ResumeThread(th_info->threadId);
}


//-----------------------------------------------------------------------------
void iosThreadInit()
{
    int tid;
    struct IosThreadInfo *info = &iosThreadDestroyMgrThread;
    void *destroyMgrEntry = iosThreadDestroyMgr;
    
    info->param.entry             = iosThreadMain;
    info->entry                   = destroyMgrEntry;
    info->param.stack             = iosThreadDestroyMgrStack;
    info->param.initPriority      = IOS_DESTROY_MGR_THREAD_PRIORITY;
    info->param.stackSize         = IOS_DESTROY_MGR_STACK_SIZE - IOS_STACK_MARK_SIZE;
    info->param.gpReg             = (&_gp);
    info->param.currentPriority   = IOS_DESTROY_MGR_THREAD_PRIORITY;

    memcpy(info->param.stack , stack_mark_begin, IOS_STACK_MARK_SIZE);
    memcpy(info->param.stack + IOS_TASK_STACK_SIZE - IOS_STACK_MARK_SIZE, stack_mark_end, IOS_STACK_MARK_SIZE);

    tid = iosThreadCreateCommon(info, NULL);

    info->flags   &= ~IOS_THRF_ALLOCATED_STACK;
    info->hasMsgQ  = 0;
    
    iosThreadStart(&iosThreadDestroyMgrThread);
}

//-----------------------------------------------------------------------------
void iosThreadCreate(
    struct IosThreadInfo* th_info,
    int unk,
    iosThreadFuncPtr entry,
    void *args,
    unsigned char* stack_ptr,
    long stack_size,
    int priority)
{
    UNUSED(unk);

    th_info->param.entry   = iosThreadMain;
    th_info->entry         = entry;
    th_info->param.stack   = stack_ptr;

    memcpy(stack_ptr, stack_mark_begin, IOS_STACK_MARK_SIZE);
    memcpy(stack_ptr + stack_size - IOS_STACK_MARK_SIZE, stack_mark_end, IOS_STACK_MARK_SIZE);

    th_info->param.stackSize       = stack_size - IOS_STACK_MARK_SIZE;
    th_info->param.gpReg           = &_gp;
    th_info->param.initPriority    = priority;
    th_info->param.currentPriority = priority;

    iosThreadCreateCommon(th_info, args);

    th_info->hasMsgQ  = 0;
    th_info->flags   &= ~IOS_THRF_ALLOCATED_STACK;
}

//-----------------------------------------------------------------------------
int iosThreadGetPri(struct IosThreadInfo *info)
{
    if (info == NULL) {
        info = thread_table[GetThreadId()];
    }

    return info->param.currentPriority;
}

//-----------------------------------------------------------------------------
struct IosThreadInfo *iosGetIOSThreadFromId(u32 tid)
{
    if (tid > MAX_THREADS) {
        debug_StdPrintfDummy("thr:id out of range\n");
        return  NULL;
    }
    else {
        return thread_table[tid];
    }
}

//-----------------------------------------------------------------------------
int iosThreadWakeup(struct IosThreadInfo const *th_info)
{
    return WakeupThread(th_info->threadId);
}

//-----------------------------------------------------------------------------
int iosThreadJoin(struct IosThreadInfo *th_info)
{
    struct ThreadMsgData *heapPtr;
    IosMessage msg;

    if (th_info->hasMsgQ == 0) {
        th_info->hasMsgQ = 1;
        heapPtr = (struct ThreadMsgData *)iosMallocDebug(
            ios_partition_root, sizeof(struct ThreadMsgData), "ios/thread.c", 0x1fa);
            th_info->msgQ = &heapPtr->queue;
        iosMsgQueueCreate(&heapPtr->queue, &heapPtr->storage, IOS_THREAD_MESSAGES_MAX);
    }

    iosMsgRecv(th_info->msgQ, &msg, 1);
    debug_StdPrintfDummy("th:thread joined\n");

    return msg;
}

//-----------------------------------------------------------------------------
int iosThreadCancelWakeup(struct IosThreadInfo const *th_info)
{
    int const tid = (th_info == NULL) ? GetThreadId() : th_info->threadId;
    return CancelWakeupThread(tid);
}

//-----------------------------------------------------------------------------
int iosSemaCreate(struct IosSemaInfo *sema_info, int init_cnt, int max_cnt, int option)
{
    int sid;

    (sema_info->param).initCount = init_cnt;
    (sema_info->param).maxCount  = max_cnt;
    (sema_info->param).option    = option;

    sid = CreateSema(&sema_info->param);
    sema_info->sid = sid;
    
    if (sema_info->sid < 0) {
        CRASH(__assert_cond_0, "ios/thread.c", 0x25c, "sem: can't create %d\n", sema_info->sid);
        return sema_info->sid;
    }

    return 0;
}

//-----------------------------------------------------------------------------
int iosSemaDelete(struct IosSemaInfo *sema_info)
{
    int const result = DeleteSema(sema_info->sid);
    if (result < 0) {
        CRASH(__assert_cond_0, "ios/thread.c", 0x270, "sem: can't delete %d\n", sema_info->sid);
        return result;
    }

    return 0;
}

//-----------------------------------------------------------------------------
int iosSemaWait(struct IosSemaInfo *sema_info)
{
    int const sid = ReferSemaStatus(sema_info->sid, &sema_info->param);
    if (sid < 0) {
        debug_StdPrintfDummy("sem: wait error? %d\n", sema_info->sid);
        return sid;
    }
    WaitSema(sema_info->sid);

    return 0;
}

//-----------------------------------------------------------------------------
int iosSemaSignal(struct IosSemaInfo *sema_info)
{
    int const sid = SignalSema(sema_info->sid);
    if (sid < 0) {
        debug_StdPrintfDummy("sem: signal error? %d\n", sema_info->sid);
        return sid;
    }

    return 0;
}

//-----------------------------------------------------------------------------
int iosSemaReferStatus(struct IosSemaInfo *sema_info)
{
    int const result = ReferSemaStatus(sema_info->sid, &sema_info->status);
    if (result < 0) {
        CRASH(__assert_cond_0, "ios/thread.c", 0x2b0, "sem: refer error? %d\n", sema_info->sid);
        return result;
    }

    return 0;
}

//-----------------------------------------------------------------------------
void iosThreadDestroyMgr(void *args)
{
    IosMessage msg;
    int tid;
    
    debug_StdPrintfDummy("iosThreadDestroyMgr() in\n");
    iosMsgQueueCreate(&iosThreadDestroyMgrMsgQ, iosThreadDestroyMgrMsgBuf, 2);

    while (1)
    {
        int hasStack = IOS_THRF_ALLOCATED_STACK;
        
        iosMsgRecv(&iosThreadDestroyMgrMsgQ, &msg, 1);
        tid = ((struct IosThreadInfo *)msg)->threadId;
        
        n_thread = n_thread - 1;
        debug_StdPrintfDummy("1:n_thread %d\n", n_thread);

        TerminateThread(tid);
        DeleteThread(tid);

        // Free thread stack memory
        if ((((struct IosThreadInfo *)msg)->flags & IOS_THRF_ALLOCATED_STACK) == hasStack)
        {
            iosFree(thread_table[tid]->param.stack);
        }

        // Free message queue memory
        if (((struct IosThreadInfo *)msg)->hasMsgQ != 0)
        {
            iosMsgQueueDestroy(((struct IosThreadInfo *)msg)->msgQ);
            iosFree(((struct IosThreadInfo *)msg)->msgQ);
        }

        thread_table[tid] = NULL;
    }
}

//-----------------------------------------------------------------------------
void iosThreadAllQuit(int skip_tid)
{
    int tid;
    for (tid = 0; tid < MAX_THREADS; ++tid) {
        struct IosThreadInfo *th_info = thread_table[tid];
        if ((th_info != NULL) && (tid != skip_tid)) {
            iosThreadDestroy(th_info);
        }
    }
}

#ifndef ICO2_FUMI_H_
#define ICO2_FUMI_H_

#include <ee/eekernel.h>

#define IOS_COMMON_TASK_PIORITY_HI      (0x21)
#define IOS_COMMON_TASK_PIORITY_LO      (0x22)

typedef void(*iosThreadFuncPtr)(void *);

enum ThreadFlags
{
    IOS_THRF_ALLOCATED_STACK    = (1 << 0u),
};

struct IosThreadInfo
{
    struct ThreadParam      param;          // x00
    s32                     threadId;       // x30
    void                   *args;           // x34
    iosThreadFuncPtr        entry;          // x38
    s32                     flags;          // x3C
    s32                     lowPrio;        // x40
    s32                     x44;            // x44
    s32                     hasMsgQ;        // x48
    struct IosMsgQueue     *msgQ;           // x4C
    u8                      name[0x20];     // x50
};
STATIC_ASSERT(sizeof(struct IosThreadInfo) == 0x70);

struct IosSemaInfo
{
    struct SemaParam        param;          // x00
    struct SemaParam        status;         // x18
    s32                     sid;            // x30
};
STATIC_ASSERT(sizeof(struct IosSemaInfo) == 0x34);

struct IosMsgQueue
{
    void                   *ptr;            // x00 - msg storage
    s32                     msgIndex;       // x04
    s32                     numMessages;    // x08
    s32                     maxMessages;    // x0C
    struct deq_mes_ptr     *x10;            // x10
    struct SemaParam        param;          // x14
    s32                     sid;            // x2c
};
STATIC_ASSERT(sizeof(struct IosMsgQueue) == 0x30);

/* memory */
extern void *ios_partition_root;
extern void *ios_partition_event;
extern void *ios_partition_isys;
extern void *ios_partition_sugipon;
extern void *ios_partition_common;
extern void *ios_partition_dmotion;
extern void *ios_partition_s2motion;
extern void *ios_partition_seki;
extern void *ios_partition_oomori;
extern void *ios_partition_sound;
extern void *ios_partition_inflate;
extern void *ios_partition_sound_semi;
extern void *ios_partition_smotion;

u8 *iosMallocDebug(u8*, int, const char*, int);
void iosFree(void *ptr);

/* thread */
extern u32 n_thread;

void iosThreadSetPri( struct IosThreadInfo *th_info, int priority);

/* message */
void iosMsgQueueCreate(struct IosMsgQueue *queue, void *storage, int max_messages);
void iosMsgQueueDestroy(struct IosMsgQueue const *queue);
int iosMsgSend(struct IosMsgQueue *queue, void *msg, int do_wait);
int iosMsgRecv(struct IosMsgQueue *queue, void *msg, int do_wait);

#endif // ICO2_FUMI_H_

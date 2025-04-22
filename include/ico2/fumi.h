#ifndef ICO2_FUMI_H_
#define ICO2_FUMI_H_

#include <ee/eekernel.h>

#include "common.h"

typedef void(*iosThreadFuncPtr)(void *);
struct iosMsgQueue;

struct IosThreadInfo
{
    struct ThreadParam      param;      // video_mode
    s32                     threadId;   // x30
    void                   *args;       // x34
    iosThreadFuncPtr        entry;      // x38
    u32                     flags;      // x3C
    s32                     lowPrio;    // x40
    s32                     x44;        // x44
    s32                     hasMsgQ;    // x48
    struct iosMsgQueue     *msgQ;       // x4C
    u8                      name[0x20]; // x50
};
STATIC_ASSERT(sizeof(struct IosThreadInfo) == 0x70);

#endif // ICO2_FUMI_H_
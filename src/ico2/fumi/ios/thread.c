#include <ico2/fumi.h>

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadMain);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadCreateS);

//-----------------------------------------------------------------------------
void iosThreadStart(struct IosThreadInfo *th_info)
{
    StartThread(th_info->threadId, th_info->args);
}

//-----------------------------------------------------------------------------
void iosThreadStop(struct IosThreadInfo *th_info)
{
    if (th_info == NULL) {
        ExitThread();
        return;
    }

    TerminateThread(th_info->threadId);
}

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadSleep);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadDestroy);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadSetPri);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadMessage);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadName);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadSuspend);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadResume);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadInit);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadCreate);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadGetPri);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosGetIOSThreadFromId);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadWakeup);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadJoin);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadCancelWakeup);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosSemaCreate);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosSemaDelete);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosSemaWait);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosSemaSignal);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosSemaReferStatus);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadDestroyMgr);

INCLUDE_ASM("asm/nonmatchings/ico2/fumi/ios/thread", iosThreadAllQuit);

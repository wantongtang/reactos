#ifndef __NTOSKRNL_INCLUDE_INTERNAL_KE_H
#define __NTOSKRNL_INCLUDE_INTERNAL_KE_H

/* INCLUDES *****************************************************************/

#include "arch/ke.h"

/* INTERNAL KERNEL TYPES ****************************************************/

typedef struct _WOW64_PROCESS
{
  PVOID Wow64;
} WOW64_PROCESS, *PWOW64_PROCESS;

/* INTERNAL KERNEL FUNCTIONS ************************************************/

struct _KIRQ_TRAPFRAME;
struct _KPCR;
struct _KPRCB;
struct _KEXCEPTION_FRAME;

extern PVOID KeUserApcDispatcher;
extern PVOID KeUserCallbackDispatcher;
extern PVOID KeUserExceptionDispatcher;
extern PVOID KeRaiseUserExceptionDispatcher;

#define IPI_REQUEST_FUNCTIONCALL    0
#define IPI_REQUEST_APC		    1
#define IPI_REQUEST_DPC		    2
#define IPI_REQUEST_FREEZE	    3

/* MACROS *************************************************************************/

#define KeEnterCriticalRegion() \
{ \
    PKTHREAD _Thread = KeGetCurrentThread(); \
    if (_Thread) _Thread->KernelApcDisable--; \
}

#define KeLeaveCriticalRegion() \
{ \
    PKTHREAD _Thread = KeGetCurrentThread(); \
    if((_Thread) && (++_Thread->KernelApcDisable == 0)) \
    { \
        if (!IsListEmpty(&_Thread->ApcState.ApcListHead[KernelMode])) \
        { \
            KiKernelApcDeliveryCheck(); \
        } \
    } \
}

/* threadsch.c ********************************************************************/

/* Thread Scheduler Functions */

/* Readies a Thread for Execution. */
VOID
STDCALL
KiDispatchThreadNoLock(ULONG NewThreadStatus);

/* Readies a Thread for Execution. */
VOID
STDCALL
KiDispatchThread(ULONG NewThreadStatus);

/* Puts a Thread into a block state. */
VOID
STDCALL
KiBlockThread(PNTSTATUS Status,
              UCHAR Alertable,
              ULONG WaitMode,
              UCHAR WaitReason);

/* Removes a thread out of a block state. */
VOID
STDCALL
KiUnblockThread(PKTHREAD Thread,
                PNTSTATUS WaitStatus,
                KPRIORITY Increment);

NTSTATUS
STDCALL
KeSuspendThread(PKTHREAD Thread);

NTSTATUS
FASTCALL
KiSwapContext(PKTHREAD NewThread);

VOID
STDCALL
KiAdjustQuantumThread(IN PKTHREAD Thread);

/* gmutex.c ********************************************************************/

VOID
FASTCALL
KiAcquireGuardedMutexContented(PKGUARDED_MUTEX GuardedMutex);

/* gate.c **********************************************************************/

VOID
FASTCALL
KeInitializeGate(PKGATE Gate);

VOID
FASTCALL
KeSignalGateBoostPriority(PKGATE Gate);

VOID
FASTCALL
KeWaitForGate(PKGATE Gate,
              KWAIT_REASON WaitReason,
              KPROCESSOR_MODE WaitMode);

/* ipi.c ********************************************************************/

BOOLEAN STDCALL
KiIpiServiceRoutine(IN PKTRAP_FRAME TrapFrame,
		    IN struct _KEXCEPTION_FRAME* ExceptionFrame);

VOID
KiIpiSendRequest(KAFFINITY TargetSet,
		 ULONG IpiRequest);

VOID
KeIpiGenericCall(VOID (STDCALL *WorkerRoutine)(PVOID),
		 PVOID Argument);

/* next file ***************************************************************/

typedef struct _KPROFILE_SOURCE_OBJECT
{
    KPROFILE_SOURCE Source;
    LIST_ENTRY ListEntry;
} KPROFILE_SOURCE_OBJECT, *PKPROFILE_SOURCE_OBJECT;

/* Cached modules from the loader block */
typedef enum _CACHED_MODULE_TYPE
{
    AnsiCodepage,
    OemCodepage,
    UnicodeCasemap,
    SystemRegistry,
    HardwareRegistry,
    MaximumCachedModuleType,
} CACHED_MODULE_TYPE, *PCACHED_MODULE_TYPE;
extern PLOADER_MODULE CachedModules[MaximumCachedModuleType];

VOID 
STDCALL
DbgBreakPointNoBugCheck(VOID);

VOID
STDCALL
KeInitializeProfile(struct _KPROFILE* Profile,
                    struct _KPROCESS* Process,
                    PVOID ImageBase,
                    ULONG ImageSize,
                    ULONG BucketSize,
                    KPROFILE_SOURCE ProfileSource,
                    KAFFINITY Affinity);

VOID
STDCALL
KeStartProfile(struct _KPROFILE* Profile,
               PVOID Buffer);

VOID
STDCALL
KeStopProfile(struct _KPROFILE* Profile);

ULONG
STDCALL
KeQueryIntervalProfile(KPROFILE_SOURCE ProfileSource);

VOID
STDCALL
KeSetIntervalProfile(KPROFILE_SOURCE ProfileSource,
                     ULONG Interval);

VOID
STDCALL
KeProfileInterrupt(
    PKTRAP_FRAME TrapFrame
);

VOID
STDCALL
KeProfileInterruptWithSource(
	IN PKTRAP_FRAME   		TrapFrame,
	IN KPROFILE_SOURCE		Source
);

BOOLEAN
STDCALL
KiRosPrintAddress(PVOID Address);

VOID STDCALL KeUpdateSystemTime(PKTRAP_FRAME TrapFrame, KIRQL Irql);
VOID STDCALL KeUpdateRunTime(PKTRAP_FRAME TrapFrame, KIRQL Irql);

VOID STDCALL KiExpireTimers(PKDPC Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);

KIRQL inline FASTCALL KeAcquireDispatcherDatabaseLock(VOID);
VOID inline FASTCALL KeAcquireDispatcherDatabaseLockAtDpcLevel(VOID);
VOID inline FASTCALL KeReleaseDispatcherDatabaseLock(KIRQL Irql);
VOID inline FASTCALL KeReleaseDispatcherDatabaseLockFromDpcLevel(VOID);

VOID
STDCALL
KeInitializeThread(struct _KPROCESS* Process,
                   PKTHREAD Thread,
                   PKSYSTEM_ROUTINE SystemRoutine,
                   PKSTART_ROUTINE StartRoutine,
                   PVOID StartContext,
                   PCONTEXT Context,
                   PVOID Teb,
                   PVOID KernelStack);

VOID
STDCALL
KeRundownThread(VOID);

NTSTATUS KeReleaseThread(PKTHREAD Thread);

LONG
STDCALL
KeQueryBasePriorityThread(IN PKTHREAD Thread);

VOID
STDCALL
KiSetPriorityThread(PKTHREAD Thread,
                    KPRIORITY Priority,
                    PBOOLEAN Released);

VOID
STDCALL
KeStackAttachProcess (
    IN struct _KPROCESS* Process,
    OUT PKAPC_STATE ApcState
    );

VOID
STDCALL
KeUnstackDetachProcess (
    IN PKAPC_STATE ApcState
    );

BOOLEAN KiDispatcherObjectWake(DISPATCHER_HEADER* hdr, KPRIORITY increment);
VOID STDCALL KeExpireTimers(PKDPC Apc,
			    PVOID Arg1,
			    PVOID Arg2,
			    PVOID Arg3);
VOID inline FASTCALL KeInitializeDispatcherHeader(DISPATCHER_HEADER* Header, ULONG Type,
 				  ULONG Size, ULONG SignalState);
VOID KeDumpStackFrames(PULONG Frame);
BOOLEAN KiTestAlert(VOID);

VOID
FASTCALL
KiAbortWaitThread(PKTHREAD Thread,
                  NTSTATUS WaitStatus,
                  KPRIORITY Increment);

VOID
STDCALL
KeInitializeProcess(struct _KPROCESS *Process,
                    KPRIORITY Priority,
                    KAFFINITY Affinity,
                    LARGE_INTEGER DirectoryTableBase);

ULONG
STDCALL
KeForceResumeThread(IN PKTHREAD Thread);

BOOLEAN
STDCALL
KeDisableThreadApcQueueing(IN PKTHREAD Thread);

BOOLEAN STDCALL KiInsertTimer(PKTIMER Timer, LARGE_INTEGER DueTime);

VOID inline FASTCALL KiSatisfyObjectWait(PDISPATCHER_HEADER Object, PKTHREAD Thread);

BOOLEAN inline FASTCALL KiIsObjectSignaled(PDISPATCHER_HEADER Object, PKTHREAD Thread);

VOID inline FASTCALL KiSatisifyMultipleObjectWaits(PKWAIT_BLOCK WaitBlock);

VOID FASTCALL KiWaitTest(PDISPATCHER_HEADER Object, KPRIORITY Increment);

PULONG KeGetStackTopThread(struct _ETHREAD* Thread);
BOOLEAN STDCALL KeContextToTrapFrame(PCONTEXT Context, PKEXCEPTION_FRAME ExeptionFrame, PKTRAP_FRAME TrapFrame);
VOID STDCALL KiDeliverApc(KPROCESSOR_MODE PreviousMode,
                  PVOID Reserved,
                  PKTRAP_FRAME TrapFrame);
VOID
STDCALL
KiKernelApcDeliveryCheck(VOID);
LONG
STDCALL
KiInsertQueue(IN PKQUEUE Queue,
              IN PLIST_ENTRY Entry,
              BOOLEAN Head);

ULONG
STDCALL
KeSetProcess(struct _KPROCESS* Process,
             KPRIORITY Increment);


VOID STDCALL KeInitializeEventPair(PKEVENT_PAIR EventPair);

VOID STDCALL KiInitializeUserApc(IN PKEXCEPTION_FRAME Reserved,
			 IN PKTRAP_FRAME TrapFrame,
			 IN PKNORMAL_ROUTINE NormalRoutine,
			 IN PVOID NormalContext,
			 IN PVOID SystemArgument1,
			 IN PVOID SystemArgument2);

PLIST_ENTRY
STDCALL
KeFlushQueueApc(IN PKTHREAD Thread,
                IN KPROCESSOR_MODE PreviousMode);


VOID STDCALL KiAttachProcess(struct _KTHREAD *Thread, struct _KPROCESS *Process, KIRQL ApcLock, struct _KAPC_STATE *SavedApcState);

VOID STDCALL KiSwapProcess(struct _KPROCESS *NewProcess, struct _KPROCESS *OldProcess);

BOOLEAN
STDCALL
KeTestAlertThread(IN KPROCESSOR_MODE AlertMode);

BOOLEAN STDCALL KeRemoveQueueApc (PKAPC Apc);
VOID FASTCALL KiWakeQueue(IN PKQUEUE Queue);
PLIST_ENTRY STDCALL KeRundownQueue(IN PKQUEUE Queue);

extern LARGE_INTEGER SystemBootTime;

/* INITIALIZATION FUNCTIONS *************************************************/

extern ULONG_PTR KERNEL_BASE;

VOID KeInitExceptions(VOID);
VOID KeInitInterrupts(VOID);
VOID KeInitTimer(VOID);
VOID KeInitDpc(struct _KPRCB* Prcb);
VOID KeInitDispatcher(VOID);
VOID inline FASTCALL KeInitializeDispatcher(VOID);
VOID KiInitializeSystemClock(VOID);
VOID KiInitializeBugCheck(VOID);
VOID Phase1Initialization(PVOID Context);

VOID KeInit1(PCHAR CommandLine, PULONG LastKernelAddress);
VOID KeInit2(VOID);

BOOLEAN KiDeliverUserApc(PKTRAP_FRAME TrapFrame);

VOID
STDCALL
KiMoveApcState (PKAPC_STATE OldState,
		PKAPC_STATE NewState);

VOID
KiAddProfileEvent(KPROFILE_SOURCE Source, ULONG Pc);
VOID
NTAPI
KiDispatchException(PEXCEPTION_RECORD ExceptionRecord,
            PKEXCEPTION_FRAME ExceptionFrame,
            PKTRAP_FRAME Tf,
		    KPROCESSOR_MODE PreviousMode,
		    BOOLEAN SearchFrames);
VOID KeTrapFrameToContext(PKTRAP_FRAME TrapFrame,
			  PCONTEXT Context);
VOID
KeApplicationProcessorInit(VOID);
VOID
KePrepareForApplicationProcessorInit(ULONG id);
ULONG
KiUserTrapHandler(PKTRAP_FRAME Tf, ULONG ExceptionNr, PVOID Cr2);
VOID STDCALL
KePushAndStackSwitchAndSysRet(ULONG Push, PVOID NewStack);
VOID STDCALL
KeStackSwitchAndRet(PVOID NewStack);
VOID STDCALL
KeBugCheckWithTf(ULONG BugCheckCode,
		 ULONG BugCheckParameter1,
		 ULONG BugCheckParameter2,
		 ULONG BugCheckParameter3,
		 ULONG BugCheckParameter4,
		 PKTRAP_FRAME Tf);
#define KEBUGCHECKWITHTF(a,b,c,d,e,f) DbgPrint("KeBugCheckWithTf at %s:%i\n",__FILE__,__LINE__), KeBugCheckWithTf(a,b,c,d,e,f)
VOID
KiDumpTrapFrame(PKTRAP_FRAME Tf, ULONG ExceptionNr, ULONG cr2);

VOID
STDCALL
KeFlushCurrentTb(VOID);

/* REACTOS SPECIFIC */

VOID STDCALL
KeRosDumpStackFrames(
  PULONG  Frame,
  ULONG  FrameCount);

ULONG STDCALL
KeRosGetStackFrames(
  PULONG  Frames,
  ULONG  FrameCount);
  
VOID
KiSetSystemTime(PLARGE_INTEGER NewSystemTime);

#define MAXIMUM_PROCESSORS      32

#endif /* __NTOSKRNL_INCLUDE_INTERNAL_KE_H */

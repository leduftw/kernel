# kernel

A preemptive multithreaded kernel for 8086 real mode, written in Borland C++.

It provides threads, counting semaphores with timed wait, and events bound to
hardware interrupts — on a processor with no protection, no MMU, and no
operating system underneath to help. Preemption comes from hooking the timer
interrupt and switching stacks by hand.

## The idea

On an 8086 there is nothing to build on. There is one execution context, a
real-mode segmented address space every thread can scribble over, and an
interrupt vector table sitting at physical address zero. A "thread" only
exists because the kernel decides to swap `ss:sp` out from under the CPU
while it isn't looking.

The whole system rests on one hook. `Timer` saves the original handler for
interrupt `8h` — IRQ0, the programmable interval timer, firing at ~18.2 Hz —
and installs its own:

```cpp
oldInterrupt = getvect(8);
setvect(8, timer);
```

Every tick, the handler chains to the original routine (DOS still needs its
clock), ages any timed semaphore waits, decrements the running thread's
remaining slice, and calls `dispatch()` when it hits zero. Because this
happens in an interrupt, the CPU has already pushed the full register set
onto the current thread's stack — so a context switch is just recording
`ss:sp` in the outgoing PCB, loading them from the incoming one, and
returning from the interrupt into a different thread's saved state.

Time is measured in units of 55 ms, one PIT tick. A thread created with
`timeSlice == 0` is never preempted and runs until it blocks or finishes.

## API

Three classes are user-facing. Each is a thin handle over a kernel-side
implementation it forward-declares but never exposes — `Thread` over `PCB`,
`Semaphore` over `KernelSem`, `Event` over `KernelEv`.

### Thread

```cpp
class Thread {
public:
    void start();
    void waitToComplete();
    ID   getId();
    static ID getRunningId();
    static Thread* getThreadById(ID id);
protected:
    Thread(StackSize stackSize = 4096, Time timeSlice = 2);
    virtual void run() { }
};
```

Subclass it, override `run()`, call `start()`. A thread moves through six
states: `CREATED` before `start()`, `READY` while queued in the scheduler,
`BLOCKED` on a semaphore, `WAITING` inside `waitToComplete()`, `FINISHED`
once `run()` returns, and `IDLE`, reserved for the idle thread that exists so
the dispatcher always has something to run when every other thread is blocked.

There is deliberately no `RUNNING` state — the running thread is whichever one
`PCB::running` points at, so it can't disagree with the state field.

### Semaphore

```cpp
int wait(Time maxTimeToWait);
int signal(int n = 0);
```

The timeout is the interesting part. `wait(0)` blocks indefinitely; anything
else blocks for at most that many ticks. The timer handler walks every blocked
thread on every semaphore each tick, decrements its remaining wait, and on
expiry unblocks the thread with `0` as the return value from `wait` — so the
caller can distinguish being signalled from timing out. A negative semaphore
value means `abs(value)` threads are queued on it.

### Event

An `Event` binds a thread to a hardware interrupt, letting it block until the
device fires. Wiring a handler into the 256-entry IVT is boilerplate, so it's
generated:

```cpp
PREPAREENTRY(numEntry, callOld)
```

This declares an interrupt routine for that vector, constructs an `IVTEntry`
that saves the previous handler and installs the new one, and defines the
routine to signal the event — optionally chaining to the original handler
first, which matters for vectors DOS is still using.

## Layout

```
h/                public headers and their kernel-side counterparts
src/              implementations
scheduler/        prebuilt scheduler (APPLICAT.LIB, SCHEDULE.H)
javni_test_2019/  test harness: bounded buffer, keyboard events, interrupt locking
```

`Kernel` owns global state and the semaphore registry. `PCBList` and `SemList`
are hand-rolled intrusive lists — there is no standard library worth speaking
of here. Critical sections are two macros:

```cpp
#define lock   asm { pushf; cli }
#define unlock asm popf
```

Saving flags rather than blindly re-enabling means these nest correctly, which
matters because most of this code can be reached from an interrupt handler.

The scheduler ships as a compiled library rather than source: the kernel is
written against the `Scheduler::put` / `Scheduler::get` interface, so the
queueing policy is deliberately swappable and not part of this implementation.

## Building

Borland C++ 3.1 or Turbo C++ for DOS, targeting real mode. This will not build
on a modern toolchain — it depends on `interrupt` functions, inline `asm`,
`getvect`/`setvect` from `<dos.h>`, and a flat real-mode address space. DOSBox
with Borland C++ installed is the practical way to run it today.

The `#ifndef BCC_BLOCK_IGNORE` guards exist to keep the Borland IDE's parser
away from inline assembly it mishandles; they have no effect on the build.

## Origin

Coursework for Operating Systems 1 at the School of Electrical Engineering,
University of Belgrade. Kept here because the exercise — preemptive
multitasking with nothing underneath you — is worth more than most of what
it taught along the way.

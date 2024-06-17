const std = @import("std");
const bro = @import("root").bro;

const Atomic = std.atomic.Value;
const ArrayList = std.ArrayList(*Task);
const Task = bro.ThreadPool.Task;
const Queue = std.fifo.LinearFifo(*Task, .Dynamic);
const Mutex = std.Thread.Mutex;
const Futex = std.Thread.Futex;

pub const EventLoop = struct {
    queue: Queue = Queue.init(bro.allocator), 
    mutex: Mutex = .{},
    state: Atomic(u32) = Atomic(u32).init(pending),

    const pending = 0;
    const notified = 1;
    const woken = 2;

    pub fn Init() EventLoop {
        return .{};
    }

    pub fn deinit(self: *EventLoop) void {
        self.queue.deinit();
        self.* = undefined;
    }

    pub fn enqueue(self: *EventLoop, task: *Task) void {
        self.mutex.lock();
        self.queue.writeItem(task) catch unreachable;
        self.mutex.unlock();
        self.notify();
    }

    pub fn loop(self: *EventLoop) void {
        while (true) {
            self.wake();
            self.tick();
            self.sleep();
        }
    }

    fn tick(self: *EventLoop) void {
        self.mutex.lock();
        while (self.queue.readItem()) |task| {
            task.callback(task);
        }
        self.mutex.unlock();
    }

    fn wake(self: *EventLoop) void {
        self.state.store(woken, .release);
    }

    fn sleep(self: *EventLoop) void {
        if (self.state.cmpxchgStrong(woken, pending, .release, .acquire) == null) {
            Futex.wait(&self.state, pending);
        }
    }

    fn notify(self: *EventLoop) void {
        const old_state = self.state.swap(notified, .release);
        if (old_state == pending) {
            Futex.wake(&self.state, 1);
        }
    }
};

fn NewEventLoop() type {
    return struct {
        var loop: EventLoop = undefined;
        var loaded: bool = false;

        fn create() *EventLoop {
            @setCold(true);
            loop = EventLoop.Init();
            return &loop;
        }

        pub inline fn get() *EventLoop {
            // TODO: lil racy
            if (loaded) return &loop;
            loaded = true;

            return create();
        }

        pub fn enqueue(task: *Task) void {
            get().enqueue(task);
        }
    };
}

pub const GlobalEventLoop = NewEventLoop();
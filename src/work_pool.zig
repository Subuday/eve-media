const std = @import("std");
const bro = @import("root").bro;

const ThreadPool = bro.ThreadPool;
const Batch = ThreadPool.Batch;

pub const Task = ThreadPool.Task;

pub fn NewWorkPool() type {
    return struct {
        var pool: ThreadPool = undefined;
        var loaded: bool = false;

        fn create() *ThreadPool {
            @setCold(true);
            pool = ThreadPool.init(.{
                .max_threads = @max(@as(u32, @truncate(std.Thread.getCpuCount() catch 0)), 2),
            });
            return &pool;
        }

        pub inline fn get() *ThreadPool {
            // TODO: lil racy
            if (loaded) return &pool;
            loaded = true;

            return create();
        }

        pub fn schedule(task: *Task) void {
            get().schedule(Batch.from(task));
        }
    };
}

pub const WorkPool = NewWorkPool();

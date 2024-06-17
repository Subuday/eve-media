const std = @import("std");
const bro = @import("root").bro;

const Atomic = std.atomic.Value;
const allocator = bro.allocator;
const c = bro.c;
const BLContextCore = bro.c.BLContextCore;
const BLImageCore = bro.c.BLImageCore;
const Task = bro.ThreadPool.Task;
const View = bro.View;
const pool = bro.work_pool;
const loop = bro.event_loop;
const frame_duration = bro.frame_duration;
const ns_per_ms = std.time.ns_per_ms;

pub const FrameScheduler = struct {
    const w = 320;
    const h = 240;
    
    img: BLImageCore = undefined,
    ctx: BLContextCore = undefined,
    frame: [w * h]u16 = [_]u16{0} ** (w * h),
    vsync_task: Task = .{ .callback = &simulateVSYNC },
    draw_task: Task = .{ .callback = &draw },
    view: *View = undefined,

    pub fn init() FrameScheduler {
        return .{};
    }

    pub fn deinit(self: *FrameScheduler) void {
        _ = c.blImageDestroy(&self.img);
        _ = c.blContextDestroy(&self.ctx);
        self.* = undefined;
    }
    
    pub fn prepare(self: *FrameScheduler) void {
        c.Gpu_init();
        _ = c.blImageInitAs(&self.img, w, h, c.BL_FORMAT_PRGB32);
        _ = c.blContextInitAs(&self.ctx, &self.img, null);
    }

    pub fn start(self: *FrameScheduler) void {
        _ = c.blContextBegin(&self.ctx, &self.img, null);
        pool.schedule(&self.vsync_task);
    }

    pub fn stop(self: *FrameScheduler) void {
        _ = c.blContextEnd(&self.ctx);
        self.vsync_task.token.cancel();
        self.vsync_task.token.join();
    }

    fn draw(task: *Task) void {
        const self = @as(*FrameScheduler, @fieldParentPtr("draw_task", task));

        _ = c.blContextSetFillStyleRgba32(&self.ctx, 0x0000FF);
        _ = c.blContextFillAll(&self.ctx);

        self.view.draw(&self.ctx);
        
        _ = c.blContextFlush(&self.ctx, c.BL_CONTEXT_FLUSH_SYNC);

        var img_data = c.BLImageData{};
        _ = c.blImageGetData(&self.img, &img_data);
        const stride: usize = @intCast(img_data.stride);
        const pixels: [*]u8 = @ptrCast(img_data.pixelData);

        for (0..h) |row| {
            for (0..w) |col| {
                const pixel = pixels + row * stride + col * 4;
                const r = pixel[0];
                const g = pixel[1];
                const b = pixel[2];

                const r5: u16 = @intCast(r >> 3);
                const g6: u16 = @intCast(g >> 2);
                const b5: u16 = @intCast(b >> 3);
                self.frame[row * w + col] = (r5 << 11) | (g6 << 5) | b5;
            }
        }

        c.Gpu_post(&self.frame);

        _ = c.blContextClearAll(&self.ctx);
    }

    pub fn simulateVSYNC(task: *Task) void {
        const self = @as(*FrameScheduler, @fieldParentPtr("vsync_task", task));
        while (!task.token.cancelled()) {
            std.time.sleep(frame_duration * ns_per_ms);
            loop.enqueue(&self.draw_task);
        }
        task.token.finish();
    }
};

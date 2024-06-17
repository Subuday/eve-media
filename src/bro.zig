pub const anim = @import("./ui/animation.zig");
const std = @import("std");
const streaming = @import("./streaming.zig");
const wp = @import("./work_pool.zig");
const el = @import("./event_loop.zig");

pub const ButtonClient = @import("./button_client.zig");
pub const EyesFeature = @import("./eyes.zig");
pub const MicStreamingFeature = streaming.MicStreamingFeature;
pub const SpeakStreamingFeature = streaming.SpeakStreamingFeature;
pub const ThreadPool = @import("./thread_pool.zig");
pub const View = @import("./ui/view.zig");

pub const c = @cImport({
    @cInclude("GpuC.h");
    @cInclude("NetworkClientC.h");
    @cInclude("MediaClientC.h");
    @cInclude("blend2d.h");
    @cInclude("bcm2835.h");
});

pub const allocator = std.heap.c_allocator;

pub const work_pool = wp.WorkPool;

pub const event_loop = el.GlobalEventLoop;

pub const frame_duration = 32; // ms


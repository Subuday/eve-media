const std = @import("std");
const bro = @import("root").bro;

const c = bro.c;
const ButtonClient = bro.ButtonClient;
const Timer = std.time.Timer;
const ns_per_ms = std.time.ns_per_ms;

pub const MicStreamingFeature = struct {
    timer: ?Timer = null,
    listener: Listener = undefined,

    pub fn Init() MicStreamingFeature {
        return MicStreamingFeature{};
    }

    pub fn prepare(self: *MicStreamingFeature) void {
        c.recorder_set_on_read_callback(self, &onAudioRead);
    }
    
    pub fn start(self: *MicStreamingFeature) void {
        c.net_send_start_recording();
        c.recorder_start();
        self.listener.onStartRecording(self);
    }

    pub fn stop(self: *MicStreamingFeature) void {
        c.recorder_stop();
        std.time.sleep(100 * ns_per_ms);
        c.net_send_stop_recording();
        self.listener.onStopRecording(self);
    }

    pub fn release() void {

    }

    fn onAudioRead(data: [*c]i8, size: c_int, ctx: ?*anyopaque) callconv(.C) void {
        _ = ctx;
        c.net_send_audio(data, size);
    }

    pub const Listener = struct {
        onStartRecording: *const (fn(*MicStreamingFeature) void),
        onStopRecording: *const (fn(*MicStreamingFeature) void),
    };
};

pub const SpeakStreamingFeature = struct {
    pub fn Init() SpeakStreamingFeature {
        return SpeakStreamingFeature{};
    }

    pub fn play(self: *SpeakStreamingFeature, data: []i8) void {
        c.player_write(data.ptr, @as(c_int, @intCast(data.len)));
        _ = self;
    }
};
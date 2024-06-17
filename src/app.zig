const App = @This();

const std = @import("std");
const bro = @import("root").bro;
const ui_sync = @import("./ui/synchronization.zig");

const c = bro.c;
const ButtonClient = bro.ButtonClient;
const FrameScheduler = ui_sync.FrameScheduler;
const MicStreamingFeature = bro.MicStreamingFeature;
const SpeakStreamingFeature = bro.SpeakStreamingFeature;
const EyesFeature = bro.EyesFeature;
const EventLoop = bro.EventLoop; 
const Task = bro.ThreadPool.Task;
const View = bro.View;

const pool = bro.pool;
const loop = bro.event_loop;

view: View,
frame_scheduler: FrameScheduler,
eyes_feature: EyesFeature,
mic_feature: MicStreamingFeature,
speak_feature: SpeakStreamingFeature,
buttons: ButtonClient,

pub fn init() App {
    return .{
        .view = View.init(),
        .frame_scheduler = FrameScheduler.init(),
        .eyes_feature = EyesFeature.init(),
        .mic_feature = MicStreamingFeature.Init(),
        .speak_feature = SpeakStreamingFeature.Init(),
        .buttons = ButtonClient.Init(),
    };
}

pub fn run(self: *App) void {
    self.frame_scheduler.view = &self.view;
    self.frame_scheduler.prepare();

    c.net_init();
    c.media_init();

    self.bind();

    self.frame_scheduler.start();
    c.net_start();
    c.media_start();
    self.buttons.start();
    self.mic_feature.prepare();

    self.eyes_feature.idle();

    loop.get().loop();
}

fn bind(self: *App) void {
    c.net_set_on_receive_audio_callback(self, &onReceiveAudio);
    self.buttons.setListener(.{ .onRecordPress = onRecordPress });
    self.mic_feature.listener = .{
        .onStartRecording = &onStartRecording,
        .onStopRecording = &onStopRecording,
    };
    self.eyes_feature.listener = .{ .onStateChange = onEyesStateChange };
    self.view.listener = .{ .onAnimationEnd = onAnimationEnd };
}

fn onRecordPress(buttons: *ButtonClient, pressed: bool) void {
    var self = @as(*App, @fieldParentPtr("buttons", buttons));
    if (pressed) {
        self.mic_feature.start();
    } else {
        self.mic_feature.stop();
    }
}

fn onReceiveAudio(data: [*c]i8, size: c_int, ctx: ?*anyopaque) callconv(.C) void {
    const self: *App = @alignCast(@ptrCast(ctx));
    self.speak_feature.play(data[0..@as(usize, @intCast(size))]);
}

fn onEyesStateChange(eyes: *EyesFeature, state: EyesFeature.State) void {
    var self = @as(*App, @fieldParentPtr("eyes_feature", eyes));
    self.view.playState(state);
}

fn onAnimationEnd(view: *View) void {
    var self = @as(*App, @fieldParentPtr("view", view));
    self.eyes_feature.next();
}

fn onStartRecording(mic: *MicStreamingFeature) void {
    var self = @as(*App, @fieldParentPtr("mic_feature", mic));
    self.eyes_feature.listen();
}

fn onStopRecording(mic: *MicStreamingFeature) void {
    var self = @as(*App, @fieldParentPtr("mic_feature", mic));
    self.eyes_feature.idle();
} 

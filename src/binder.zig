const std = @import("std");
const bro = @import("root").bro;
const Binder = @This();

const c = bro.c;
const ButtonClient = bro.ButtonClient;
const MicStreamingFeature = bro.MicStreamingFeature;
const SpeakStreamingFeature = bro.SpeakStreamingFeature;
const EyesFeature = bro.EyesFeature;
const View = bro.View;

button_client: *ButtonClient,
mic_streaming: *MicStreamingFeature,
speak_streaming: *SpeakStreamingFeature,
eyes: *EyesFeature,
view: *View,

pub fn bind(self: *Binder) void {
    c.net_set_on_receive_audio_callback(self, &onReceiveAudio);
    self.button_client.setListener(.{ .onRecordPress = onRecordPress });
    self.eyes.listener = .{ .onStateChange = onEyesStateChange };
}

fn onRecordPress(client: *ButtonClient, pressed: bool) void {
    var self = @as(*Binder, @fieldParentPtr("button_client", @constCast(&client)));
    if (pressed) {
        self.mic_streaming.start();
    } else {
        self.mic_streaming.stop();
    }
    std.debug.print("Record button pressed {}\n", .{ pressed });
}

fn onReceiveAudio(data: [*c]i8, size: c_int, ctx: ?*anyopaque) callconv(.C) void {
    const self: *Binder = @alignCast(@ptrCast(ctx));
    self.speak_streaming.play(data[0..@as(usize, @intCast(size))]);
}

fn onEyesStateChange(eyes: *EyesFeature, state: EyesFeature.State) void {
    var self = @as(*Binder, @fieldParentPtr("eyes", @constCast(&eyes)));
    self.view.playState(state);
}
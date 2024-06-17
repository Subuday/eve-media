const std = @import("std");
const bro = @import("root").bro;
const View = @This();

const BLContextCore = bro.c.BLContextCore;
const FrameSet = bro.anim.FrameSet;
const NoneFrameSet = bro.anim.NoneFrameSet;
const IdleFrameSet = bro.anim.IdleFrameSet;
const ClippingFrameSet = bro.anim.ClippingFrameSet;
const ListeningFrameSet = bro.anim.ListeningFrameSet;
const State = bro.EyesFeature.State;
 
idle_frame_set: IdleFrameSet,
listening_frame_set: ListeningFrameSet,
clipping_frame_set: ClippingFrameSet,

frame_set: ?FrameSet = null,
listener: Listener = undefined,

pub fn init() View {
    return .{
        .idle_frame_set = IdleFrameSet.init(),
        .listening_frame_set = ListeningFrameSet.init(),
        .clipping_frame_set = ClippingFrameSet.init(),
    };
}

pub fn deinit(self: *View) void {
    self.idle_frame_set.deinit();
    self.listening_frame_set.deinit();
    self.clipping_frame_set.deinit();
    self.* = undefined;
}

pub fn playState(self: *View, state: State) void {
    switch (state) {
        .none => {
            self.frame_set = null;
            return;
        },
        .idle => {
            self.frame_set = self.idle_frame_set.frameSet();
        },
        .listening => {
            self.frame_set = self.listening_frame_set.frameSet();
        },
        .clipping => {
            self.frame_set = self.clipping_frame_set.frameSet();
        }
    }
    self.frame_set.?.anim.reset();
}

pub fn draw(self: *View, ctx: *BLContextCore) void {
    if (self.frame_set == null) {
        return;
    }
    self.frame_set.?.drawFrame(ctx);
    if (self.frame_set.?.anim.ended()) {
        self.listener.onAnimationEnd(self);
    }
}

pub const Listener = struct {
    onAnimationEnd: *const (fn (*View) void),
};
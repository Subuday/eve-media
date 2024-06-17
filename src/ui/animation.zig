const std = @import("std");
const bro = @import("root").bro;

const blend = bro.c;

const BLContextCore = blend.BLContextCore;

const dt: u32 = bro.frame_duration;

const width = 320;
const height = 240;
const margin_top = 80;
const margin_horizontal = 66;

const eye_width = 66;
const eye_height = 80;

const def_e1_cx = margin_horizontal + eye_width / 2;
const def_e1_cy = margin_top + eye_height / 2;
const def_e1_x1 = margin_horizontal;
const def_e1_y1 = margin_top;
const def_e1_x2 = def_e1_x1 + eye_width;
const def_e1_y2 = def_e1_y1 + eye_height;

const def_e2_cx = width - margin_horizontal - eye_width / 2;
const def_e2_cy = margin_top + eye_height / 2;
const def_e2_x1 = width - margin_horizontal - eye_width;
const def_e2_y1 = margin_top;
const def_e2_x2 = def_e2_x1 + eye_width;
const def_e2_y2 = def_e2_y1 + eye_height;

const Eye = struct {
    w: u32 = eye_width,
    h: u32 = eye_height,
    cx: i32,
    cy: i32,
    rotation: f64 = 0,
    rx: u32 = 8,
    ry: u32 = 8,
    color: u32 = 0xFFFFFFFF,

    fn x1(self: *Eye) i32 {
        return self.cx - @as(i32, @intCast(self.w / 2));
    }

    fn y1(self: *Eye) i32 {
        return self.cy - @as(i32, @intCast(self.h / 2));
    }

    fn x2(self: *Eye) i32 {
        return self.cx + @as(i32, @intCast(self.w / 2));
    }

    fn y2(self: *Eye) i32 {
        return self.cy + @as(i32, @intCast(self.h / 2));
    }
};

const Eyes = struct {
    e1: Eye = .{
        .cx = def_e1_cx,
        .cy = def_e1_cy,
    },
    e2: Eye = .{
        .cx = def_e2_cx,
        .cy = def_e2_cy,
    },
};

fn drawEye(ctx: *BLContextCore, eye: *Eye) void {
    var cookie: blend.BLContextCookie = .{};
    _ = blend.blContextSave(ctx, &cookie);

    const rotation_data = [_]f64{ std.math.degreesToRadians(eye.rotation), @floatFromInt(eye.cx), @floatFromInt(eye.cy) };
    _ = blend.blContextApplyTransformOp(ctx, blend.BL_TRANSFORM_OP_ROTATE_PT, &rotation_data);

    const rect = blend.BLRoundRect {
        .x = @floatFromInt(eye.x1()),
        .y = @floatFromInt(eye.y1()),
        .w = @floatFromInt(eye.w),
        .h = @floatFromInt(eye.h),
        .rx = @floatFromInt(eye.rx),
        .ry = @floatFromInt(eye.ry),
    };
    _ = blend.blContextFillGeometryRgba32(ctx, blend.BL_GEOMETRY_TYPE_ROUND_RECT, &rect, eye.color);

    _ = blend.blContextRestore(ctx, &cookie);
}

fn drawEyes(ctx: *BLContextCore, eyes: *Eyes) void {
    drawEye(ctx, &eyes.e1);
    drawEye(ctx, &eyes.e2);
}

pub const FrameSet = struct {
    ptr: *anyopaque,
    anim: *IntAnimator,
    drawFramePtr: *const (fn (ptr: *anyopaque, ctx: *BLContextCore) void),

    pub fn drawFrame(self: *FrameSet, ctx: *BLContextCore) void {
        (self.drawFramePtr)(self.ptr, ctx);
    }
};

pub const IdleFrameSet = struct {
    var key_frames: [5]IntAnimator.KeyFrame = [_]IntAnimator.KeyFrame{
        IntAnimator.KeyFrame.init(def_e1_cy, 0),
        IntAnimator.KeyFrame.init(def_e1_cy + 2, 150),
        IntAnimator.KeyFrame.init(def_e1_cy, 300),
        IntAnimator.KeyFrame.init(def_e1_cy - 2, 450),
        IntAnimator.KeyFrame.init(def_e1_cy, 600),
    };
    eyes: Eyes = .{},
    anim: IntAnimator,

    pub fn init() IdleFrameSet {
        return .{ .anim = IntAnimator.init(&key_frames) };
    }

    pub fn frameSet(self: *IdleFrameSet) FrameSet {
        return .{
            .ptr = self,
            .anim = &self.anim,
            .drawFramePtr = &drawFrame,
        };
    }

    fn drawFrame(ptr: *anyopaque, ctx: *BLContextCore) void  {
        const self: *IdleFrameSet = @ptrCast(@alignCast(ptr));
        const cy = self.anim.eval();
        self.eyes.e1.cy = cy;
        self.eyes.e2.cy = cy;
        drawEyes(ctx, &self.eyes);
    }
};

pub const ClippingFrameSet = struct {
    var key_frames: [3]IntAnimator.KeyFrame = [_]IntAnimator.KeyFrame{
        IntAnimator.KeyFrame.init(eye_height, 0),
        IntAnimator.KeyFrame.init(0, 300),
        IntAnimator.KeyFrame.init(eye_height, 600),
    };
    eyes: Eyes = .{},
    anim: IntAnimator,

    pub fn init() ClippingFrameSet {
        return .{ .anim = IntAnimator.init(&key_frames) };
    }

    pub fn frameSet(self: *ClippingFrameSet) FrameSet {
        return .{
            .ptr = self,
            .anim = &self.anim,
            .drawFramePtr = &drawFrame,
        };
    }

    fn drawFrame(ptr: *anyopaque, ctx: *BLContextCore) void  {
        const self: *ClippingFrameSet = @ptrCast(@alignCast(ptr));
        const h: u32 = @intCast(self.anim.eval());
        self.eyes.e1.h = h;
        self.eyes.e2.h = h;
        drawEyes(ctx, &self.eyes);
    }
};

pub const ListeningFrameSet = struct {
    var key_frames: [4]IntAnimator.KeyFrame = [_]IntAnimator.KeyFrame{
        IntAnimator.KeyFrame.init(92, 0),
        IntAnimator.KeyFrame.init(92, 1600),
        IntAnimator.KeyFrame.init(0, 2400),
        IntAnimator.KeyFrame.init(92, 3200),
    };
    eyes: Eyes = .{
        .e1 = .{ 
            .w = 76, // w * 1.16
            .h = 92, // h * 1.16
            .cx = def_e1_cx,
            .cy = def_e1_cy,
            .rotation = -12,
        },
        .e2 = .{
            .w = 76, // w * 1.16
            .h = 92, // h * 1.16
            .cx = def_e2_cx,
            .cy = def_e2_cy - 12,
            .rotation = -12,
        },
    },
    anim: IntAnimator,

    pub fn init() ListeningFrameSet {
        return .{ .anim = IntAnimator.init(&key_frames) };
    }

    pub fn drawFrame(ptr: *anyopaque, ctx: *BLContextCore) void  {
        const self: *ListeningFrameSet = @ptrCast(@alignCast(ptr));
        const h: u32 = @intCast(self.anim.eval());
        self.eyes.e1.h = h;
        self.eyes.e2.h = h;
        drawEyes(ctx, &self.eyes);
    }

    pub fn frameSet(self: *ListeningFrameSet) FrameSet {
        return .{
            .ptr = self,
            .anim = &self.anim,
            .drawFramePtr = &drawFrame,
        };
    }
};

pub const SpeakingFrameSet = struct {
    var key_frames: [3]IntAnimator.KeyFrame = [_]IntAnimator.KeyFrame{
        IntAnimator.KeyFrame.init(eye_height, 0),
        IntAnimator.KeyFrame.init(0, 300),
        IntAnimator.KeyFrame.init(eye_height, 600),
    };
    eyes: Eyes = .{},
    anim: IntAnimator,

    pub fn init() SpeakingFrameSet {
        return .{ .anim = IntAnimator.init(&key_frames) };
    }

    pub fn drawFrame(self: *SpeakingFrameSet, ctx: *BLContextCore) void  {
        const h: u32 = @intCast(self.anim.eval());
        self.eyes.e1.h = h;
        self.eyes.e2.h = h;
        drawEyes(ctx, &self.eyes);
    }
};

const IntAnimator = struct {
    key_frames: []KeyFrame,
    key_frame_index: u32 = 0,
    frames: u32,
    frame_index: u32 = 0,

    const KeyFrame = struct {
        i: u32,
        v: i32,

        fn init(v: i32, t: u32) KeyFrame {
            var frames = t / dt;
            if (t % dt != 0 or frames == 0) {
                frames += 1;
            }
            return .{
                .i = frames - 1,
                .v = v
            };
        }
    };

    fn init(key_frames: []KeyFrame) IntAnimator {
        return .{
            .key_frames = key_frames,
            .frames = key_frames[key_frames.len - 1].i + 1,
        };
    }

    fn eval(self: *IntAnimator) i32 {
        const key_frame = self.key_frames[self.key_frame_index];
        const next_key_frame = self.key_frames[self.key_frame_index + 1];

        const segment_frame_index = self.frame_index - key_frame.i;
        const segment_frames = next_key_frame.i - key_frame.i + 1;

        const initial_value = key_frame.v;
        const target_value = next_key_frame.v;
        const fraction = LinearInterpolation(@as(f32, @floatFromInt(segment_frame_index)) / @as(f32, @floatFromInt(segment_frames - 1)));
        const step: i32 = @intFromFloat(fraction * @as(f32, @floatFromInt(target_value - initial_value)));
        
        const value = key_frame.v + step;

        if (self.frame_index == next_key_frame.i) {
            if (self.key_frame_index != self.key_frames.len - 2) {
                self.key_frame_index += 1;
            }
        }

        if (self.frame_index != self.frames - 1) {
            self.frame_index += 1;
        }

        return value;
    }

    pub fn reset(self: *IntAnimator) void {
        self.key_frame_index = 0;
        self.frame_index = 0;
    }

    pub fn ended(self: *IntAnimator) bool {
        return self.frame_index == self.frames - 1;
    }
};

fn LinearInterpolation(input: f32) f32 {
    return input;
}
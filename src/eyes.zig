const std = @import("std");
const EyesFeature = @This();

state: State = .none,
seq_count: u32 = 0,
listener: Listener = undefined,

pub fn init() EyesFeature {
    return .{};
}

pub fn deinit(self: *EyesFeature) void {
    self.* = undefined;
}

pub fn idle(self: *EyesFeature) void {
    self.updateState(.idle);
}

pub fn clip(self: *EyesFeature) void {
    self.updateState(.clipping);
}

pub fn listen(self: *EyesFeature) void {
    self.updateState(.listening);
}

pub fn next(self: *EyesFeature) void {
    self.seq_count += 1;
    if (self.state == .idle and self.seq_count == 2) {
        self.clip();
        return;
    } else if (self.state == .clipping and self.seq_count == 1) {
        self.idle();
        return;
    }
    self.listener.onStateChange(self, self.state);
}

fn updateState(self: *EyesFeature, state: State) void {
    self.seq_count = 0;
    self.state = state;
    self.listener.onStateChange(self, self.state);
}

pub const State = enum(u8) {
    none,
    idle,
    listening,
    clipping,
};

pub const Listener = struct {
    onStateChange: *const (fn (*EyesFeature, State) void),
};
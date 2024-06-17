const std = @import("std");
const bro = @import("root").bro;
const ButtonClient = @This();

const c = bro.c;
const ns_per_ms = std.time.ns_per_ms;
const pool = bro.work_pool;
const Task = bro.ThreadPool.Task;

task: Task = .{ .callback = &startRecordButtonPollingLoop },
listener: ?Listener = null,

pub fn Init() ButtonClient {
    if(c.bcm2835_init() != 1) {
        @panic("bcm2835_init failed. Are you running as root?");
    }
    setupRecordButton();
    return .{};
}

pub fn start(self: *ButtonClient) void {
    pool.schedule(&self.task);
}

pub fn stop(self: *ButtonClient) void {
    self.task.token.cancel();
    self.task.token.join();
}

pub fn deinit(self: *ButtonClient) void {
    _ = c.bcm2835_close();
    self.* = undefined;
}

pub fn setListener(self: *ButtonClient, listener: Listener) void {
    self.listener = listener;
}

fn setupRecordButton() void {
    c.bcm2835_gpio_fsel(c.RPI_BPLUS_GPIO_J8_37, c.BCM2835_GPIO_FSEL_INPT);
    c.bcm2835_gpio_set_pud(c.RPI_BPLUS_GPIO_J8_37, c.BCM2835_GPIO_PUD_UP);
}

fn startRecordButtonPollingLoop(task: *Task) void {
    const self = @as(*ButtonClient, @fieldParentPtr("task", task));
    var pressed = false;
    while (!task.token.cancelled()) {
        if (c.bcm2835_gpio_lev(c.RPI_BPLUS_GPIO_J8_37) == c.LOW) {
            if (!pressed) {
                pressed = true;
                self.listener.?.onRecordPress(self, pressed);
            }
        } else {
            if (pressed) {
                pressed = false;
                self.listener.?.onRecordPress(self, pressed);
            }
        }
        std.time.sleep(100 * ns_per_ms);
    }
    task.token.finish();
}

pub const Listener = struct {
    onRecordPress: *const fn(*ButtonClient, bool) void,
};
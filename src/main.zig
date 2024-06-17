const std = @import("std");
const App = @import("app.zig");
const drawer = @import("./ui/animation.zig");
const bro = @import("root").bro;

const blend = bro.c;

const gpu = @cImport({
    @cInclude("GpuC.h");
});

const BLContextCore = blend.BLContextCore;

const EyesClippingDrawer = drawer.EyesClippingDrawer;
const EyesShakeDrawer = drawer.EyesShakeDrawer;
const IdleFrameSet = drawer.IdleFrameSet;
const ClippingFrameSet = drawer.ClippingFrameSet;
const ListeningFrameSet = drawer.ListeningFrameSet;
const SpeakingFrameSet = drawer.SpeakingFrameSet;

pub fn main() !void {
    var app = App.init();
    app.run();
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();

    // const allocator = gpa.allocator();

    var img: blend.BLImageCore = undefined;
    var ctx: BLContextCore = undefined;

    _ = blend.blImageInitAs(&img, 320, 240, blend.BL_FORMAT_PRGB32);
    _ = blend.blContextInitAs(&ctx, &img, null);
    _ = blend.blContextBegin(&ctx, &img, null);

    // var eyes = ListeningFrameSet.init();

    // std.debug.print("Pixel at ({}, {}) = ({})\n", .{h, w, stride});

    // std.debug.print("Hello, {s}!\n", .{"World"});

    gpu.Gpu_init();

    while (true) {
        _ = blend.blContextSetFillStyleRgba32(&ctx, 0x000000FF);
        _ = blend.blContextFillAll(&ctx);

        // eyes.drawFrame(&ctx);

        _ = blend.blContextFlush(&ctx, blend.BL_CONTEXT_FLUSH_SYNC);

        var codec = blend.BLImageCodecCore{};
        _ = blend.blImageCodecInitByName(&codec, "PNG", blend.SIZE_MAX, null);
        _ = blend.blImageWriteToFile(&img, "eyes_output.png", &codec);

        // var imgData = blend.BLImageData{};
        // _ = blend.blImageGetData(&img, &imgData);

        // const h: usize = @intCast(imgData.size.h);
        // const w: usize = @intCast(imgData.size.w);
        // const stride: usize = @intCast(imgData.stride);
        // const pixels: [*]u8 = @ptrCast(imgData.pixelData);

        // const frame = try allocator.alloc(u16, w * h);
        // defer allocator.free(frame);

        // for (0..h) |row| {
        //     for (0..w) |col| {
        //         const pixel = pixels + row * stride + col * 4;
        //         const r = pixel[0];
        //         const g = pixel[1];
        //         const b = pixel[2];

        //         const r5: u16 = @intCast(r >> 3);
        //         const g6: u16 = @intCast(g >> 2);
        //         const b5: u16 = @intCast(b >> 3);
        //         frame[row * w + col] = (r5 << 11) | (g6 << 5) | b5;

        //         // std.debug.print("Pixel at ({}, {}) - ({}, {}, {})\n", .{row, col, r5, g6, b5});
        //     }
        // }

        // gpu.Gpu_post(frame.ptr);

        _ = blend.blContextClearAll(&ctx);

        std.time.sleep(16 * 1_000_000);
    }

    _ = blend.blContextEnd(&ctx);
    _ = blend.blContextDestroy(&ctx);
    _ = blend.blImageDestroy(&img);
}

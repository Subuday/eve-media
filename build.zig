const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const thread_pool = b.addObject(.{
        .name = "thread_pool",
        .root_source_file = .{ .path = "src/thread_pool.zig" },
        .target = target,
        .optimize = optimize,
    });

    const eve = b.addExecutable(.{
        .name = "eve",
        .root_source_file = .{ .path = "root.zig" },
        .target = target,
        .optimize = optimize,
    });
    eve.linkLibC();
    eve.linkLibCpp();
    eve.addObject(thread_pool);
    eve.addIncludePath(.{ .path = "include" });
    eve.addIncludePath(.{ .path = "lib/bcm2835" });
    eve.addIncludePath(.{ .path = "lib/blend2d/include" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/board/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/config/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/display/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/gpio/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/lcd_driver/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/render/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/spi/" });
    eve.addCSourceFiles(.{
        .files = &.{ "src/MediaClient.cpp", "src/NetworkClient.cpp", "src/Session.cpp", "src/utils/Semaphore.cpp", "src/utils/CountDownLatch.cpp", "src/App.cpp", "src/c/GpuC.cpp", "src/c/NetworkClientC.cpp", "src/c/MediaClientC.cpp", "lib/eve-lcd-driver/src/display/diff.cpp", "lib/eve-lcd-driver/src/display/display.cpp", "lib/eve-lcd-driver/src/display/gpu.cpp", "lib/eve-lcd-driver/src/display/mailbox.cpp", "lib/eve-lcd-driver/src/display/mem_alloc.cpp", "lib/eve-lcd-driver/src/display/spi.cpp", "lib/eve-lcd-driver/src/display/statistics.cpp", "lib/eve-lcd-driver/src/display/text.cpp", "lib/eve-lcd-driver/src/gpio/gpio_utils.cpp", "lib/eve-lcd-driver/src/lcd_driver/st7789.cpp", "lib/eve-lcd-driver/src/render/Gpu.cpp", "lib/eve-lcd-driver/src/render/Surface.cpp", "lib/eve-lcd-driver/src/render/Vsync.cpp", "lib/eve-lcd-driver/src/spi/spi_utils.cpp", "lib/eve-lcd-driver/src/st7789V.cpp" },
        .flags = &.{ "-std=c++17", "-DSPI_BUS_CLOCK_DIVISOR=20", "-DWAVESHARE_2INCH_LCD=ON", "-DST7789", "-DST7789VW", "-DBACKLIGHT_CONTROL=OFF", "-DSTATISTICS=1", "-DNO_TLS" },
    });
    eve.addCSourceFiles(.{ .files = &.{
        "lib/bcm2835/bcm2835.c",
    } });
    // eve.linkSystemLibrary("atomic");
    eve.linkSystemLibrary("pthread");
    eve.linkSystemLibrary("bcm_host");
    eve.linkSystemLibrary("pulse");
    eve.linkSystemLibrary("ssl");
    eve.linkSystemLibrary("crypto");
    eve.linkSystemLibrary("boost_system");
    eve.addObjectFile(.{ .path = "lib/blend2d/libblend2d.a" });
    eve.linkSystemLibrary("rt");
    b.installArtifact(eve);
}

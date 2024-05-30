const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const eve = b.addExecutable(.{
        .name = "eve",
        .root_source_file = null,
        .target = target,
        .optimize = optimize,
    });
    eve.linkLibC();
    eve.linkLibCpp();
    
    eve.addIncludePath(.{ .path = "include" });
    eve.addIncludePath(.{ .path = "lib/bcm2835" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/board/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/config/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/display/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/gpio/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/lcd_driver/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/render/" });
    eve.addIncludePath(.{ .path = "lib/eve-lcd-driver/src/spi/" });
    eve.addCSourceFiles(.{
        .files = &.{
            "src/main.cpp",
            "src/App.cpp",
            "src/MediaClient.cpp",
            "src/NetworkClient.cpp",
            "src/Session.cpp",
            "src/ButtonClient.cpp",
            "src/MicAudioStreamingUseCase.cpp",
            "src/SpeakerAudioStreamingUseCase.cpp",
            "src/ManageEyesStateUseCase.cpp",
            "src/ShutdownListenerUseCase.cpp",
            "src/SignalListenerUseCase.cpp",
            "src/EyesView.cpp",
            "src/utils/Semaphore.cpp",
            "src/utils/CountDownLatch.cpp",
            "src/utils/Timer.cpp",

            "lib/eve-lcd-driver/src/display/diff.cpp",
            "lib/eve-lcd-driver/src/display/display.cpp",
            "lib/eve-lcd-driver/src/display/gpu.cpp",
            "lib/eve-lcd-driver/src/display/mailbox.cpp",
            "lib/eve-lcd-driver/src/display/mem_alloc.cpp",
            "lib/eve-lcd-driver/src/display/spi.cpp",
            "lib/eve-lcd-driver/src/display/statistics.cpp",
            "lib/eve-lcd-driver/src/display/text.cpp",
            "lib/eve-lcd-driver/src/gpio/gpio_utils.cpp",
            "lib/eve-lcd-driver/src/lcd_driver/st7789.cpp",
            "lib/eve-lcd-driver/src/render/Gpu.cpp",
            "lib/eve-lcd-driver/src/render/Surface.cpp",
            "lib/eve-lcd-driver/src/render/Vsync.cpp",
            "lib/eve-lcd-driver/src/spi/spi_utils.cpp",
            "lib/eve-lcd-driver/src/st7789V.cpp"
        },
        .flags = &.{
            "-std=c++17",
            "-DSPI_BUS_CLOCK_DIVISOR=20",
            "-DWAVESHARE_2INCH_LCD=ON",
            "-DST7789",
            "-DST7789VW",
            "-DBACKLIGHT_CONTROL=OFF",
            "-DSTATISTICS=1",
            "-DNO_TLS"
        },
    });
    eve.addCSourceFiles(.{
        .files = &.{
            "lib/bcm2835/bcm2835.c",
        }
    });
    // eve.linkSystemLibrary("atomic");
    eve.linkSystemLibrary("pthread");
    eve.linkSystemLibrary("bcm_host");
    eve.linkSystemLibrary("pulse");
    eve.linkSystemLibrary("ssl");
    eve.linkSystemLibrary("crypto");
    eve.linkSystemLibrary("boost_system");

    b.installArtifact(eve);
}

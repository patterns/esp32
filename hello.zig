const std = @import("std");

pub extern fn printf(
    _format: [*:0]const u8
) c_int;

pub export fn hello_zig_main(
    _argc: c_int,
    _argv: [*]const [*]const u8
) c_int {
    _ = _argc;
    _ = _argv;
    _ = printf("Hello Zig\x0A");
    return 0;
}


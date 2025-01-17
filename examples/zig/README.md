# Zig VMM example

This example aims to show using [Zig](https://ziglang.org/) to interact with
libvmm. Although Zig is most-often thought of as a programming language, the
project actually provides three things:
1. The Zig programming language
2. The Zig build system
3. A portable drop-in C/C++ cross-compiler

This example makes use of all three. The VMM code itself is written in the
Zig programming langauge, it calls into libvmm which has been compiled using
the Zig C compiler. Building the example is done via the Zig build system.

## Building the example

At the moment, Zig still under heavy development and hence this example depends
on the 'master' version of Zig for now. This example has been built using
`0.13.0-dev.365+332fbb4b0`, so anything equal to or above that version should work.

You can download Zig [here](https://ziglang.org/download/).

```sh
zig build -Dsdk=/path/to/sdk
```

To view other options available when building the example (such as optimisation
level or the target Microkit configuration), run the following command:
```sh
zig build -Dsdk=/path/to/sdk -h
```

## Running the example

To also run the example via QEMU, run the following command:
```sh
zig build -Dsdk=/path/to/sdk qemu
```

You should see the following output after a couple of seconds:
```
Welcome to Buildroot
buildroot login:
```

The login is `root`. There is no password necessary. After logging in, you will
see the console prompt where you can input commands:
```
Welcome to Buildroot
buildroot login: root
#
```

## Producing libmicrokit.zig

You will notice that in the `src/` directory there is more than just the
`vmm.zig` implementation.

For some context, the Zig toolchain has the ability to automatically convert
C headers to Zig code which can then obviously be called from Zig like any
other Zig API.

Here is the problem though, this translation fails currently for inline assembly,
which in libsel4, there is a lot of as all system calls makes use of inline assembly.

If you run step 1 below, you will encounter the following text:
```
// microkit-sdk-1.2.6/board/qemu_arm_virt/debug/include/sel4/arch/syscalls.h:490:5: warning: TODO implement translation of stmt class GCCAsmStmtClass
// microkit-sdk-1.2.6/board/qemu_arm_virt/debug/include/sel4/arch/syscalls.h:487:26: warning: unable to translate function, demoted to extern
```

Until this is solved, unfortunately using libsel4 and hence libmicrokit from Zig
is a more complicated process than it needs to be.

For now the way we deal with this is having a subset of libsel4 reproduced in
a separate translation unit such that Zig does not have to translate it since
it's not in a C header file. We can then call into the translation unit from
`libmicrokit.zig`.

### Steps to reproduce libmicrokit.zig

Sometimes a new Zig version has breaking changes so we have to re-translate
`libmicrokit.zig` (since it's so large that manually fixing each compiler error
is tedious). These are the steps for doing so.

#### Step 1

Produce the translated version of `microkit.h`.
```sh
zig translate-c <MICROKIT SDK PATH>/board/qemu_arm_virt/debug/include/microkit.h -I <MICROKIT SDK PATH>/board/qemu_arm_virt/debug/include -target aarch64-freestanding > src/libmicrokit.zig
```

#### Step 2

Add the following to the top of `libmicrokit.zig`
```zig
const libmicrokit = @cImport({
    @cInclude("libmicrokit.h");
});
```

#### Step 3

Replace `arm_sys_send_recv` with `libmicrokit.zig_arm_sys_send_recv`.
Replace `arm_sys_send` with `libmicrokit.zig_arm_sys_send`.

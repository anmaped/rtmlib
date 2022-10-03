# NuttX Demo

This demo exemplifies how rtmlib couple software monitors with NuttX RTOS on RISC-V architectures.

# Overview

This example, divided into two parts, shows the `app` application (with one task) monitored by the `app-monitor` application (with another task).

```mermaid
stateDiagram-v2
    [*] --> Demo
    Demo --> [*]

    state Demo {
        [*] --> starting_app_monitor
        starting_app_monitor --> consuming_symbols
        consuming_symbols --> consuming_symbols
        consuming_symbols --> stopping_app_monitor : Stop Event
        stopping_app_monitor --> [*]
        --
        [*] --> starting_app
        starting_app --> executing_app
        executing_app --> producing_symbols
        producing_symbols --> executing_app
        executing_app --> exiting_app : Stop Event
        exiting_app --> [*]
    }
```

# Compile and Run

Run `make` to compile inside `examples/qemu-riscv-virt-nuttx` directory. For dependencies, see the reference [Containerfile](../../Containerfile).

Run QEMU
```
qemu-system-riscv64 -M virt -cpu rv64 -m 512M -smp 8 -monitor none -nographic -serial stdio -bios none -kernel nuttx
```
The NuttX monolithic binary can be found in `./rtmlib/thirdparty/nuttx/nuttx`.

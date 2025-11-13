# RISCV-OS Lab1

## 目录结构

```
lab1/
├── Makefile
└── kernel/
    ├── defs.h
    ├── entry.S
    ├── kernel.ld
    ├── main.c
    ├── memlayout.h
    ├── riscv.h
    ├── start.c
    ├── types.h
    └── uart.c
```

## 编译方法

1. 确保已安装 `riscv64-unknown-elf-gcc` 交叉编译工具链。
2. 在 `lab1` 目录下打开终端。
3. 执行以下命令进行编译：

```sh
make
```

编译成功后会生成相应的二进制文件（如 `kernel/kernel.elf` 或 `kernel/kernel.bin`）。

## 运行方法

可以使用 QEMU 进行仿真运行。例如：

```sh
qemu-system-riscv64 -machine virt -nographic -bios none -kernel kernel/kernel.elf
```

或根据 Makefile 里的说明运行。

## 其他说明

- 所有源代码均位于 `kernel/` 目录下。
- `Makefile` 位于项目根目录，负责编译整个项目。
- 具体实现细节请参考各源码文件注释。

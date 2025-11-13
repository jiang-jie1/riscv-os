# lab2

## 目录结构

```
lab2/
├── Makefile
├── kernel/
│   ├── console.c
│   ├── defs.h
│   ├── entry.S
│   ├── kernel.ld
│   ├── main.c
│   ├── memlayout.h
│   ├── printf.c
│   ├── riscv.h
│   ├── start.c
│   ├── types.h
│   └── uart.c
```

## 编译方法

1. 进入 `lab2` 目录：
   ```
   cd lab2
   ```

2. 执行 make 命令进行编译：
   ```
   make
   ```

   编译完成后会生成 `kernel.elf` 等目标文件。

## 运行方法
  ```
  make qemu
  ```

## 说明

- 所有源代码均位于 `kernel/` 子目录下。
- `Makefile` 负责自动化编译流程。
- 运行前请确保已安装交叉编译工具链和 QEMU（如需仿真）。

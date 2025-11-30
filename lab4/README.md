# lab4: 中断处理与时钟管理实验


## 目录结构

```
lab4/
├── kernel/
│   ├── main.c           # 启动与测试入口
│   ├── pmm.c/h          # 物理内存分配器实现
│   ├── vm.c/h           # 页表管理实现
│   ├── vm_boot.c        # 虚拟内存激活
│   ├── trap.c/h         # 中断与异常处理
│   ├── timer.c/h        # 时钟管理
│   ├── ...              # 其他内核模块
├── Makefile             # 构建脚本
├── README.md            # 编译运行说明（本文件）
├── report.md            # 实验报告模板
└── ...
```


## 编译方法

1. 安装交叉编译工具链（如 riscv64-unknown-elf-gcc）
2. 进入 lab4 目录：
   ```sh
   cd lab4
   ```
3. 编译内核：
   ```sh
   make
   ```
   生成 kernel.elf


## 运行方法

### QEMU仿真

1. 安装 QEMU（推荐 qemu-system-riscv64）
2. 启动仿真：
   ```sh
   qemu-system-riscv64 -machine virt -nographic -bios none -kernel kernel.elf
   ```
   - 启动后可在串口看到内核输出和测试结果，包括中断计数、异常信息、时钟响应等。

### 物理机/开发板（如有）
- 将 kernel.elf 烧录到开发板，串口连接查看输出


## 常见问题
- 编译报错：检查工具链和环境变量
- QEMU无输出：检查 kernel.elf 是否生成、命令行参数是否正确
- 中断无响应：检查 trapvec 映射、中断使能、trap handler 注册。
- 时钟计数异常：检查 timer 初始化和中断使能。
- 异常处理卡死：确认 sepc 跳转逻辑、异常返回设置。
- 其他问题请参考实验报告“问题与解决方案”部分


## 参考
- xv6-riscv 官方仓库
- RISC-V 特权级手册
- 实验指导书

## 中断与时钟测试说明

main.c 中已集成 trapinithart、test_timer_interrupt、test_exception_handling、test_interrupt_overhead 等测试函数。
启动后可在串口看到中断计数、异常处理、时钟中断等输出。
可通过修改 main.c 进一步验证中断和异常处理流程。

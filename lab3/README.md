# lab3: 页表与内存管理实验

## 目录结构

```
lab3/
├── kernel/
│   ├── main.c           # 启动与测试入口
│   ├── pmm.c/h          # 物理内存分配器实现
│   ├── vm.c/h           # 页表管理实现
│   ├── vm_boot.c        # 虚拟内存激活
│   ├── ...              # 其他内核模块
├── Makefile             # 构建脚本
├── README.md            # 编译运行说明（本文件）
├── report.md            # 实验报告模板
└── ...
```

## 编译方法

1. 安装交叉编译工具链（如 riscv64-unknown-elf-gcc）
2. 进入 lab3 目录：
   ```sh
   cd lab3
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
   - 启动后可在串口看到内核输出和测试结果

### 物理机/开发板（如有）
- 将 kernel.elf 烧录到开发板，串口连接查看输出

## 常见问题
- 编译报错：检查工具链和环境变量
- QEMU无输出：检查 kernel.elf 是否生成、命令行参数是否正确
- 其他问题请参考实验报告“问题与解决方案”部分

## 参考
- xv6-riscv 官方仓库
- RISC-V 特权级手册
- 实验指导书

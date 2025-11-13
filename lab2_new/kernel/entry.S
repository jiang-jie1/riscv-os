.section .text
.global _entry
_entry:
    # 为单核系统直接设置栈指针
    la sp, stack0       # 加载栈基地址
    li a0, 1024*4       # 栈大小（4KB）
    add sp, sp, a0      # 设置栈顶指针（栈向低地址增长）

     # 清零.bss段（未初始化的全局变量区域）
    la a0, bss_start    # a0 = .bss段起始地址
    la a1, bss_end      # a1 = .bss段结束地址
bss_clear_loop:
    bge a0, a1, bss_clear_done  # 如果a0 >= a1，说明清零完成
    sw zero, 0(a0)              # 向当前地址写入0
    addi a0, a0, 4              # 地址向后移动4字节（RISC-V是32位架构）
    j bss_clear_loop            # 继续循环清零
bss_clear_done:

    call main          # 跳转到内核初始化函数

spin:
    j spin              # 如果main返回，进入死循环
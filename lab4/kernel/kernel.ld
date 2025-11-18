/* 最小操作系统链接脚本 */

ENTRY(_entry)

SECTIONS
{
  /* 代码段从 0x80000000 开始 */
  . = 0x80000000;

  /* 代码段 */
  .text : {
    *(.text)
    etext = .;
  }

  /* 只读数据段 */
  .rodata : {
    *(.rodata)
  }

  /* 数据段 */
  .data : {
    *(.data)
  }

  /* BSS 段（未初始化数据） */
  .bss : {
    bss_start = .;
    *(.bss)
    bss_end = .;
  }

  /* 结束地址 */
  end = .;
}
/*
******************************************************************************
**
**  File        : LinkerScript.ld
**
**  Author      : STM32CubeIDE
**
**  Abstract    : Linker script for STM32H7 series
**                2048Kbytes FLASH and 1056Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** Copyright (c) 2022 STMicroelectronics.
** All rights reserved.
**
** This software is licensed under terms that can be found in the LICENSE file
** in the root directory of this software component.
** If no LICENSE file comes with this software, it is provided AS-IS.
**
****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM_D1) + LENGTH(RAM_D1);    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x1000;      /* required amount of heap  */
_Min_Stack_Size = 0x1000; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
  FLASH (rx)     : ORIGIN = 0x08000000, LENGTH = 2048K
  DTCMRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K
  RAM_D1 (xrw)   : ORIGIN = 0x24000000, LENGTH = 512K
  RAM_D2 (xrw)   : ORIGIN = 0x30000000, LENGTH = 288K
  RAM_D3 (xrw)   : ORIGIN = 0x38000000, LENGTH = 64K
  ITCMRAM (xrw)  : ORIGIN = 0x00000000, LENGTH = 64K
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* itcmram */
  .itcmram_bss :
  {
    . = . + 1;       /* itcmram 从 0x0 开始，由于这个地址有特殊意义，所以不使用这个地址 */
    . = ALIGN(4);
    _sbss_itcm = .;         /* define a global symbol at itcmram bss start */

    *(.bss.itcmram*)

    . = ALIGN(4);
    _ebss_itcm = .;         /* define a global symbol at itcmram bss end */
  } >ITCMRAM

  _sidata_itcm = LOADADDR(.itcmram_data_text);
  .itcmram_data_text :
  {
    . = ALIGN(4);
     _sdata_itcm = .;

    *(.itcmram*)
    /* 路径里有 UserCode 的文件中的代码段 */
    /* lvgl 太大了，放不下 */
    EXCLUDE_FILE (*UserCode/Lib/lvgl*.o) *UserCode*.o(.text*)

    . = ALIGN(4);
    _edata_itcm = .;
  } >ITCMRAM AT >FLASH

  /* dtcmram */
  .dtcmram_bss :
  {
    . = ALIGN(4);
    _sbss_dtcm = .;         /* define a global symbol at dtcmram bss start */

    /* FreeRTOS 的内存池 */
    *heap_4.o(.bss.ucHeap*) 
    *(.bss.dtcmram*)

    . = ALIGN(4);
    _ebss_dtcm = .;         /* define a global symbol at dtcmram bss end */
  } >DTCMRAM

  _sidata_dtcm = LOADADDR(.dtcmram_data_text);
  .dtcmram_data_text :
  {
    . = ALIGN(4);
     _sdata_dtcm = .; 
    *(.dtcmram*)
    . = ALIGN(4);
    _edata_dtcm = .;
  } >DTCMRAM AT >FLASH

  /* ram_d1 */
  .ram_d1_bss :
  {
    . = ALIGN(4);
    _sbss_d1 = .;         /* define a global symbol at ram_d1 bss start */

    *(.bss.ram_d1*)

    . = ALIGN(4);
    _ebss_d1 = .;         /* define a global symbol at ram_d1 bss end */
  } >RAM_D1

  _sidata_d1 = LOADADDR(.ram_d1_data_text);
  .ram_d1_data_text :
  {
    . = ALIGN(4);
     _sdata_d1 = .; 
    *(.ram_d1*)
    . = ALIGN(4);
    _edata_d1 = .;
  } >RAM_D1 AT >FLASH

  /* ram_d2 */
  .ram_d2_bss :
  {
    . = ALIGN(4);
    _sbss_d2 = .;         /* define a global symbol at ram_d2 bss start */

    *lv_port_disp.o(.bss.*buf_1*)
    *lv_port_disp.o(.bss.*buf_2*)
    *(.bss.ram_d2*)

    . = ALIGN(4);
    _ebss_d2 = .;         /* define a global symbol at ram_d2 bss end */
  } >RAM_D2

  _sidata_d2 = LOADADDR(.ram_d2_data_text);
  .ram_d2_data_text :
  {
    . = ALIGN(4);
     _sdata_d2 = .; 
    *(.ram_d2*)
    . = ALIGN(4);
    _edata_d2 = .;
  } >RAM_D2 AT >FLASH

  /* ram_d3 */
  .ram_d3_bss :
  {
    . = ALIGN(4);
    _sbss_d3 = .;         /* define a global symbol at ram_d3 bss start */

    *(.bss.ram_d3*)

    . = ALIGN(4);
    _ebss_d3 = .;         /* define a global symbol at ram_d3 bss end */
  } >RAM_D3

  _sidata_d3 = LOADADDR(.ram_d3_data_text);
  .ram_d3_data_text :
  {
    . = ALIGN(4);
     _sdata_d3 = .; 
    *(.ram_d3*)
    . = ALIGN(4);
    _edata_d3 = .;
  } >RAM_D3 AT >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamFunc)        /* .RamFunc sections */
    *(.RamFunc*)       /* .RamFunc* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM_D1 AT> FLASH

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM_D1

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM_D1

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}



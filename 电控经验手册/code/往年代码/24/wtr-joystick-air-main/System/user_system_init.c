#include "main.h"

/**
 * @brief 清理内存的函数
 * @param start_pos 开始地址
 * @param end_pos 结束地址
 */
static void ClearMem(unsigned int *start_pos, unsigned int *end_pos)
{
    while (start_pos < end_pos) {
        *start_pos++ = 0;
    }
}

/**
 * @brief 将 FLASH 中的数据复制到内存中
 *
 */
static void CopyData(unsigned int *sidata, unsigned int *sdata, unsigned int *edata)
{
    while (sdata < edata) {
        *sdata++ = *sidata++;
    }
}

/**
 * @brief 用于在 main() 之前初始化内存。请在启动文件中的 `bl __libc_init_array` 之前调用此函数
 * @note 调用方法：bl __UserSystemInit
 * @note stm32 官方库自带的 startup.s 只会初始化一块内存，如果存在多个内存区域则不会初始化
 * @note 于是我修改了 startup.s，在 main() 之前调用这个函数初始化这些内存
 * @note （才不是因为不会写汇编才用 c 语言初始化内存）
 *
 */
void __UserSystemInit()
{
    __HAL_RCC_D2SRAM1_CLK_ENABLE();
    __HAL_RCC_D2SRAM2_CLK_ENABLE();
    __HAL_RCC_D2SRAM3_CLK_ENABLE();

    // 各区域bss段的开始结束地址（定义在 ld 文件中）
    extern unsigned int _sbss_d1, _sbss_d2, _sbss_d3, _sbss_itcm, _sbss_dtcm;
    extern unsigned int _ebss_d1, _ebss_d2, _ebss_d3, _ebss_itcm, _ebss_dtcm;

    // 清零各区域的 bss 段
    ClearMem(&_sbss_d1, &_ebss_d1);
    ClearMem(&_sbss_d2, &_ebss_d2);
    ClearMem(&_sbss_d3, &_ebss_d3);
    ClearMem(&_sbss_itcm, &_ebss_itcm);
    ClearMem(&_sbss_dtcm, &_ebss_dtcm);

    // 从 Flash 中复制数据到内存中
    extern unsigned int _sidata_itcm, _sdata_itcm, _edata_itcm;
    CopyData(&_sidata_itcm, &_sdata_itcm, &_edata_itcm);

    extern unsigned int _sidata_dtcm, _sdata_dtcm, _edata_dtcm;
    CopyData(&_sidata_dtcm, &_sdata_dtcm, &_edata_dtcm);

    extern unsigned int _sidata_d1, _sdata_d1, _edata_d1;
    CopyData(&_sidata_d1, &_sdata_d1, &_edata_d1);

    extern unsigned int _sidata_d2, _sdata_d2, _edata_d2;
    CopyData(&_sidata_d2, &_sdata_d2, &_edata_d2);

    extern unsigned int _sidata_d3, _sdata_d3, _edata_d3;
    CopyData(&_sidata_d3, &_sdata_d3, &_edata_d3);
}

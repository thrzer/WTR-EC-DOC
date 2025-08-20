/**
 * @file stm32_st7796.hpp
 * @author X. Y.
 * @brief
 * @version 0.1
 * @date 2023-03-31
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <stdint.h>
#include "port/lcd_fmc.hpp"
#include "port/lcd_backlight.hpp"
#include "tim.h"
#include "mdma.h"

class LcdSt7796 : public LcdFmc, public LcdBacklight
{
public:
    enum class Rotation {
        portrait,
        landscape,
        inverse_portrait,
        inverse_landscape
    };

    enum class ColorOrder {
        BGR,
        RGB
    };

private:
    ColorOrder _color_order;
    Rotation _rotation = Rotation::landscape;
    uint16_t _horizontal_resolution;
    uint16_t _vertical_resolution;
    void (*_ExternalWriteDataDmaCpltCallback)(LcdSt7796 *lcd);

    void delay(uint32_t delay_ms)
    {
        HAL_Delay(delay_ms - 1);
    }

    void updateRotationAndColorOrder();
    /**
     * @brief 设置写入区域
     * @note 屏幕的最左上角坐标为(0, 0)，最右下角坐标为(_horizontal_resolution - 1, _vertical_resolution - 1)
     * @note 区域包括顶点。例如(0, 0, 1, 1)一共有 4 个像素点
     *
     * @param x1 区域的左上角水平坐标（向右）
     * @param y1 区域的左上角垂直坐标（向下）
     * @param x2 区域的右下角水平坐标（向右）
     * @param y2 区域的右下角垂直坐标（向下）
     */
    void setDataWritingArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

    void WriteDataDmaCpltCallback() override
    {
        UnLock();
        if (_ExternalWriteDataDmaCpltCallback != nullptr) {
            _ExternalWriteDataDmaCpltCallback(this);
        }
    }

public:
    const uint16_t PHYSICAL_WIDTH  = 320;
    const uint16_t PHYSICAL_HEIGHT = 480;

    LcdSt7796(int chip_select, Rotation rotation = Rotation::landscape, ColorOrder color_order = ColorOrder::BGR, TIM_HandleTypeDef *backlight_tim = nullptr, uint32_t channel = 0, MDMA_HandleTypeDef *mdma = nullptr)
        : LcdFmc(chip_select, mdma), LcdBacklight(backlight_tim, channel), _color_order(color_order), _rotation(rotation)
    {
    }

    void InitAll()
    {
        LcdFmc::InitDma();
        InitDisplay();
        InitBacklight();
    }
    void InitDisplay();
    void InitBacklight()
    {
        LcdBacklight::Init();
        SetBacklight(0.2);
    }

    void RegisterExternalWriteScreenDmaCpltCallback(void (*ExternalWriteDataDmaCpltCallback)(LcdSt7796 *lcd))
    {
        _ExternalWriteDataDmaCpltCallback = ExternalWriteDataDmaCpltCallback;
    }
    void DisplayOnOff(bool is_display_on);
    void SetRotation(Rotation rotation);
    void SetColorOrder(ColorOrder color_order);
    ColorOrder GetColorOrder() const
    {
        return _color_order;
    }
    void InvertDisplay(bool is_invert);

    /**
     * @brief 向屏幕中指定区域写入数据
     * @note 屏幕的最左上角坐标为(0, 0)，最右下角坐标为(_horizontal_resolution - 1, _vertical_resolution - 1)
     * @note 区域包括顶点。例如(0, 0, 1, 1)一共有 4 个像素点
     * @note 像素格式：RGB565 或 BGR565，取决于 `_color_order`
     *
     * @param x1 区域的左上角水平坐标（向右）
     * @param y1 区域的左上角垂直坐标（向下）
     * @param x2 区域的右下角水平坐标（向右）
     * @param y2 区域的右下角垂直坐标（向下）
     * @param data 要写入的数据。
     */
    void WriteScreen(int x1, int y1, int x2, int y2, uint16_t *data);
    void WriteScreenDma(int x1, int y1, int x2, int y2, uint16_t *data);
    void FillScreen(uint16_t color = 0xffff);
    void FillArea(int x1, int y1, int x2, int y2, uint16_t color = 0xffff);
    int16_t GetScanline();
    uint16_t GetHorizontalResolution() const
    {
        return _horizontal_resolution;
    }

    uint16_t GetVerticalResolution() const
    {
        return _vertical_resolution;
    }
};

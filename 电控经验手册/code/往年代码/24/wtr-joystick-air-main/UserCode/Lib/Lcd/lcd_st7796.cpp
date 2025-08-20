/**
 * @file lcd_st7796.cpp
 * @author X. Y.
 * @brief
 * @version 0.1
 * @date 2023-04-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "lcd_st7796.hpp"

#define ST7796S_NOP       0x00 // No Operation
#define ST7796S_SWRESET   0x01 // Software reset
#define ST7796S_RDDID     0x04 // Read Display ID
#define ST7796S_RDNUMED   0x05 // Read Number of the Errors on DSI
#define ST7796S_RDDST     0x09 // Read Display Status
#define ST7796S_RDDPM     0x0A // Read Display Power Mode
#define ST7796S_RDDMADCTL 0x0B // Read Display MADCTL
#define ST7796S_RDDCOLMOD 0x0C // Read Display Pixel Format
#define ST7796S_RDDIM     0x0D // Read Display Image Mode
#define ST7796S_RDDSM     0x0E // Read Display Signal Status
#define ST7796S_RDDSDR    0x0F // Read Display Self-Diagnostic Result
#define ST7796S_SLPIN     0x10 // Sleep In
#define ST7796S_SLPOUT    0x11 // Sleep Out
#define ST7796S_PTLON     0x12 // Partial Display Mode On
#define ST7796S_NORON     0x13 // Normal Display Mode On
#define ST7796S_INVOFF    0x20 // Display Inversion Off
#define ST7796S_INVON     0x21 // Display Inversion On
#define ST7796S_DISPOFF   0x28 // Display Off
#define ST7796S_DISPON    0x29 // Display On
#define ST7796S_CASET     0x2A // Column Address Set
#define ST7796S_RASET     0x2B // Row Address Set
#define ST7796S_RAMWR     0x2C // Memory Write
#define ST7796S_RAMRD     0x2E // Memory Read
#define ST7796S_PTLAR     0x30 // Partial Area
#define ST7796S_VSCRDEF   0x33 // Vertical Scrolling Definition
#define ST7796S_TEOFF     0x34 // Tearing Effect Line OFF
#define ST7796S_TEON      0x35 // Tearing Effect Line On
#define ST7796S_MADCTL    0x36 // Memory Data Access Control
#define ST7796S_VSCSAD    0x37 // Vertical Scroll Start Address of RAM
#define ST7796S_IDMOFF    0x38 // Idle Mode Off
#define ST7796S_IDMON     0x39 // Idle Mode On
#define ST7796S_COLMOD    0x3A // Interface Pixel Format
#define ST7796S_WRMEMC    0x3C // Write Memory Continue
#define ST7796S_RDMEMC    0x3E // Read Memory Continue
#define ST7796S_STE       0x44 // Set Tear ScanLine
#define ST7796S_GSCAN     0x45 // Get ScanLine
#define ST7796S_WRDISBV   0x51 // Write Display Brightness
#define ST7796S_RDDISBV   0x52 // Read Display Brightness Value
#define ST7796S_WRCTRLD   0x53 // Write CTRL Display
#define ST7796S_RDCTRLD   0x54 // Read CTRL value Display
#define ST7796S_WRCABC    0x55 // Write Adaptive Brightness Control
#define ST7796S_RDCABC    0x56 // Read Content Adaptive Brightness Control
#define ST7796S_WRCABCMB  0x5E // Write CABC Minimum Brightness
#define ST7796S_RDCABCMB  0x5F // Read CABC Minimum Brightness
#define ST7796S_RDFCS     0xAA // Read First Checksum
#define ST7796S_RDCFCS    0xAF // Read Continue Checksum
#define ST7796S_RDID1     0xDA // Read ID1
#define ST7796S_RDID2     0xDB // Read ID2
#define ST7796S_RDID3     0xDC // Read ID3

#define ST7796S_IFMODE    0xB0 // Interface Mode Control
#define ST7796S_FRMCTR1   0xB1 // Frame Rate Control (In Normal Mode/Full Colors)
#define ST7796S_FRMCTR2   0xB2 // Frame Rate Control 2 (In Idle Mode/8 colors)
#define ST7796S_FRMCTR3   0xB3 // Frame Rate Control 3(In Partial Mode/Full Colors)
#define ST7796S_DIC       0xB4 // Display Inversion Control
#define ST7796S_BPC       0xB5 // Blanking Porch Control
#define ST7796S_DFC       0xB6 // Display Function Control
#define ST7796S_EM        0xB7 // Entry Mode Set
#define ST7796S_PWR1      0xC0 // Power Control 1
#define ST7796S_PWR2      0xC1 // Power Control 2
#define ST7796S_PWR3      0xC2 // Power Control 3
#define ST7796S_VCMPCTL   0xC5 // VCOM Control
#define ST7796S_VCMOST    0xC6 // VCOM Offset Register
#define ST7796S_NVMADW    0xD0 // NVM Address/Data Write
#define ST7796S_NVMBPROG  0xD1 // NVM Byte Program
#define ST7796S_NVMSTRD   0xD2 // NVM Status Read
#define ST7796S_RDID4     0xD3 // Read ID4
#define ST7796S_PGC       0xE0 // Positive Gamma Control
#define ST7796S_NGC       0xE1 // Negative Gamma Control
#define ST7796S_DGC1      0xE2 // Digital Gamma Control 1
#define ST7796S_DGC2      0xE3 // Digital Gamma Control 2
#define ST7796S_DOCA      0xE8 // Display Output Ctrl Adjust
#define ST7796S_CSCON     0xF0 // Command Set Control
#define ST7796S_SPIRC     0xFB // SPI Read Control

#define TFT_NO_ROTATION   (ST7796S_MADCTL_MV)
#define TFT_ROTATE_90     (ST7796S_MADCTL_MX)
#define TFT_ROTATE_180    (ST7796S_MADCTL_MV | ST7796S_MADCTL_MX | ST7796S_MADCTL_MY)
#define TFT_ROTATE_270    (ST7796S_MADCTL_MY)

void LcdSt7796::updateRotationAndColorOrder()
{
    uint8_t madctrl_data;
    switch (_rotation) {
        case Rotation::portrait:
            madctrl_data = 0x80;
            break;

        case Rotation::landscape:
            madctrl_data = 0x20;
            break;

        case Rotation::inverse_portrait:
            madctrl_data = 0x40;
            break;

        case Rotation::inverse_landscape:
            madctrl_data = 0xe0;
            break;

        default:
            madctrl_data = 0x20;
            break;
    }

    switch (_color_order) {
        case ColorOrder::BGR:
            madctrl_data |= 0x08;
            break;
        case ColorOrder::RGB:
            madctrl_data |= 0x00;
            break;

        default:
            break;
    }

    WriteCmd(ST7796S_MADCTL);
    WriteData(madctrl_data);
}

void LcdSt7796::setDataWritingArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    WriteCmd(ST7796S_CASET); // set col address - 4 param
    WriteData((x1 & 0xff00) >> 8);
    WriteData(x1 & 0xff);
    WriteData((x2 & 0xff00) >> 8);
    WriteData(x2 & 0xff);

    WriteCmd(ST7796S_RASET); // set page address - 4 param
    WriteData((y1 & 0xff00) >> 8);
    WriteData(y1 & 0xff);
    WriteData((y2 & 0xff00) >> 8);
    WriteData(y2 & 0xff);

    WriteCmd(ST7796S_RAMWR); // write memory start
}

void LcdSt7796::InitDisplay()
{
    Lock();
    WriteCmd(ST7796S_SWRESET);
    delay(100);

    WriteCmd(ST7796S_SLPOUT);
    delay(20);

    WriteCmd(ST7796S_CSCON);
    WriteData(0xc3); // enable command 2 part I
    WriteCmd(ST7796S_CSCON);
    WriteData(0x96); // enable command 2 part II

    WriteCmd(ST7796S_COLMOD);
    WriteData(0x55); // 0x55:16bit/pixel, 0x66:18bit/pixel

    WriteCmd(ST7796S_DIC);
    WriteData(0x01);

    WriteCmd(ST7796S_EM);
    WriteData(0xc6);

    WriteCmd(ST7796S_PWR2);
    WriteData(0x15);

    WriteCmd(ST7796S_PWR3);
    WriteData(0xaf);

    WriteCmd(ST7796S_VCMPCTL);
    WriteData(0x22);

    WriteCmd(ST7796S_VCMOST);
    WriteData(0x00);

    WriteCmd(ST7796S_DOCA);
    WriteData(0x40);
    WriteData(0x8a);
    WriteData(0x00);
    WriteData(0x00);
    WriteData(0x29);
    WriteData(0x19);
    WriteData(0xa5);
    WriteData(0x33);

    WriteCmd(ST7796S_PGC);
    WriteData(0xF0);
    WriteData(0x04);
    WriteData(0x08);
    WriteData(0x09);
    WriteData(0x08);
    WriteData(0x15);
    WriteData(0x2F);
    WriteData(0x42);
    WriteData(0x46);
    WriteData(0x28);
    WriteData(0x15);
    WriteData(0x16);
    WriteData(0x29);
    WriteData(0x2D);

    WriteCmd(ST7796S_NGC);
    WriteData(0xF0);
    WriteData(0x04);
    WriteData(0x09);
    WriteData(0x09);
    WriteData(0x08);
    WriteData(0x15);
    WriteData(0x2E);
    WriteData(0x46);
    WriteData(0x46);
    WriteData(0x28);
    WriteData(0x15);
    WriteData(0x15);
    WriteData(0x29);
    WriteData(0x2D);

    // 由于IC在横屏状态下无法垂直刷新屏幕，无法进行垂直同步，所以IC刷新率不重要了
    // WriteCmd(ST7796S_BPC);
    // WriteData(141);   // VFP, Front porch of Number lines (Min=2,Typ=8)
    // WriteData(4);    // VBP, Back porch of Number lines (Min=2,Typ=4)
    // WriteData(0x00); // All zeros
    // WriteData(4);    // HBP, Dotclk number of horizontal back porch period (Min=4)

    // /**
    //  * Set frame rate
    //  * FrameRate = 10^7 / ((168 + RTNA + 32 * (15 - FRS))*(480 + VFP + VBP))
    //  */
    // uint8_t FRS  = 11; // FRS[3:0]: Sets the frame frequency of full color normal mode.
    // uint8_t DIVA = 0;  // DIVA [1:0] : division ratio for internal clocks when Normal mode.
    // uint8_t RTNA = 24; // RTNA [4:0] : RTNA[4:0] is used to set 1H (line) period of Normal mode at CPU interface.
    // WriteCmd(ST7796S_FRMCTR1);
    // WriteData(((FRS & 0x0f) << 4) | (DIVA & 0x03));
    // WriteData(RTNA & 0x0f);

    WriteCmd(ST7796S_NORON);

    WriteCmd(ST7796S_WRCTRLD);
    WriteData(0x24);
    WriteCmd(ST7796S_CSCON);
    WriteData(0x3C);
    WriteCmd(ST7796S_CSCON);
    WriteData(0x69);

    UnLock();

    SetRotation(_rotation);
    SetColorOrder(_color_order);
    InvertDisplay(true);
    DisplayOnOff(true);
}

void LcdSt7796::DisplayOnOff(bool is_display_on)
{
    Lock();
    WriteCmd(is_display_on ? ST7796S_DISPON : ST7796S_DISPOFF);
    UnLock();
}

void LcdSt7796::SetRotation(Rotation rotation)
{
    Lock();
    _rotation = rotation;
    switch (rotation) {
        case Rotation::portrait:
            _horizontal_resolution = PHYSICAL_WIDTH;
            _vertical_resolution   = PHYSICAL_HEIGHT;
            break;
        case Rotation::landscape:
            _horizontal_resolution = PHYSICAL_HEIGHT;
            _vertical_resolution   = PHYSICAL_WIDTH;
            break;
        case Rotation::inverse_portrait:
            _horizontal_resolution = PHYSICAL_WIDTH;
            _vertical_resolution   = PHYSICAL_HEIGHT;
            break;
        case Rotation::inverse_landscape:
            _horizontal_resolution = PHYSICAL_HEIGHT;
            _vertical_resolution   = PHYSICAL_WIDTH;
            break;

        default:
            break;
    }
    updateRotationAndColorOrder();
    UnLock();
}

void LcdSt7796::SetColorOrder(ColorOrder color_order)
{
    Lock();
    _color_order = color_order;
    updateRotationAndColorOrder();
    UnLock();
}

void LcdSt7796::InvertDisplay(bool is_invert)
{
    Lock();
    WriteCmd(is_invert ? ST7796S_INVON : ST7796S_INVOFF);
    UnLock();
}

void LcdSt7796::WriteScreen(int x1, int y1, int x2, int y2, uint16_t *data)
{
    Lock();
    setDataWritingArea(x1, y1, x2, y2);
    int total = (x2 - x1 + 1) * (y2 - y1 + 1);

    while (total > 0) {
        WriteData(*data++);
        total--;
    }
    UnLock();
}

void LcdSt7796::WriteScreenDma(int x1, int y1, int x2, int y2, uint16_t *data)
{
    Lock(); // 在 WriteDataDmaCpltCallback() 中解锁
    setDataWritingArea(x1, y1, x2, y2);
    WriteDataDma(data, (x2 - x1 + 1) * (y2 - y1 + 1));
}

void LcdSt7796::FillScreen(uint16_t color)
{
    FillArea(0, 0, _horizontal_resolution - 1, _vertical_resolution - 1, color);
}

void LcdSt7796::FillArea(int x1, int y1, int x2, int y2, uint16_t color)
{
    Lock();
    setDataWritingArea(x1, y1, x2, y2);
    int total = (x2 - x1 + 1) * (y2 - y1 + 1);

    while (total > 0) {
        WriteData(color);
        total--;
    }
    UnLock();
}

int16_t LcdSt7796::GetScanline()
{
    uint16_t result;
    Lock();
    WriteCmd(ST7796S_GSCAN);
    ReadData();
    result = (ReadData() & 0x00ff) << 8;
    result |= ReadData() & 0x00ff;
    UnLock();
    return result;
}

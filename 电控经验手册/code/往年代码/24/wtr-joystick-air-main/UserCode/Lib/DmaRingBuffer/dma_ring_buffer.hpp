#pragma once

#include "dma.h"
#include <array>

template <typename _BufferType, std::size_t _BufferLength>
class DmaRingBuffer
{
private:
    std::array<_BufferType, _BufferLength> buffer_;
    DMA_HandleTypeDef *hdma_;
    int32_t buffer_start_pos = 0;

public:
    DmaRingBuffer(DMA_HandleTypeDef *hdma)
        : hdma_(hdma){};
};

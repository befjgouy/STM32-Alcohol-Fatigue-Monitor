// font.c
#include "font.h"

// 简化版：仅适配“中”“国”的索引（实际需做完整GB2312查表）
uint16_t GetHZIndex(uint8_t h_byte, uint8_t l_byte)
{
    if(h_byte == 0xD6 && l_byte == 0xD0) return 0;  // “中”
    if(h_byte == 0xB9 && l_byte == 0xFA) return 1;  // “国”
    return 0;  // 无匹配返回默认
}
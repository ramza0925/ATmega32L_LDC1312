#define FONT_OFFSET10x16   32 //first code ascii
#define FONT_WIDTH10x16    20
const unsigned char font10x16[] = {
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //  
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x0D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // !
     0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // "
     0x80,0x00,0x90,0x0C,0xD0,0x03,0xBC,0x00,0x93,0x0C,0xD0,0x03,0xBC,0x00,0x93,0x00,0x90,0x00,0x00,0x00, // #
     0x00,0x00,0x00,0x00,0x0E,0x0C,0x13,0x08,0x21,0x08,0xFF,0x1F,0xC1,0x08,0x81,0x07,0x00,0x00,0x00,0x00, // $
     0x0E,0x08,0x11,0x04,0x11,0x02,0x91,0x01,0x4E,0x00,0x20,0x07,0x98,0x08,0x84,0x08,0x82,0x08,0x01,0x07, // %
     0x80,0x03,0x40,0x04,0x2E,0x08,0x31,0x08,0xD1,0x08,0x99,0x09,0x0E,0x0E,0x00,0x0C,0x00,0x0B,0xC0,0x00, // &
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // '
     0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x03,0x0C,0x0C,0x06,0x18,0x02,0x10,0x01,0x20,0x01,0x20,0x00,0x00, // (
     0x00,0x00,0x01,0x20,0x01,0x20,0x02,0x10,0x06,0x18,0x0C,0x0C,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x00, // )
     0x00,0x00,0x04,0x00,0x0C,0x00,0x78,0x00,0x27,0x00,0x78,0x00,0x0C,0x00,0x04,0x00,0x00,0x00,0x00,0x00, // *
     0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0xF0,0x0F,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x00, // +
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4C,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ,
     0x00,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x00,0x00,0x00,0x00, // -
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // .
     0x00,0x00,0x00,0x20,0x00,0x18,0x00,0x06,0x80,0x01,0x60,0x00,0x18,0x00,0x06,0x00,0x01,0x00,0x00,0x00, // /
     0x00,0x00,0xF8,0x01,0x06,0x06,0x01,0x08,0x01,0x08,0x01,0x08,0x06,0x06,0xF8,0x01,0x00,0x00,0x00,0x00, // 0
     0x00,0x00,0x02,0x08,0x02,0x08,0x02,0x08,0xFF,0x0F,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x00,0x00,0x00, // 1
     0x00,0x00,0x03,0x0C,0x01,0x0A,0x01,0x09,0x81,0x08,0x61,0x08,0x1E,0x08,0x00,0x00,0x00,0x00,0x00,0x00, // 2
     0x00,0x00,0x00,0x00,0x01,0x08,0x21,0x08,0x21,0x08,0x21,0x08,0xDE,0x07,0x00,0x00,0x00,0x00,0x00,0x00, // 3
     0x80,0x01,0x60,0x01,0x10,0x01,0x0C,0x01,0x02,0x01,0xFF,0x0F,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00, // 4
     0x00,0x00,0x00,0x00,0x1F,0x08,0x11,0x08,0x11,0x08,0x21,0x04,0xC1,0x03,0x00,0x00,0x00,0x00,0x00,0x00, // 5
     0x00,0x00,0xF8,0x03,0x26,0x04,0x11,0x08,0x11,0x08,0x11,0x08,0x21,0x04,0xC0,0x03,0x00,0x00,0x00,0x00, // 6
     0x00,0x00,0x01,0x00,0x01,0x0C,0x81,0x03,0x61,0x00,0x19,0x00,0x05,0x00,0x03,0x00,0x00,0x00,0x00,0x00, // 7
     0x00,0x00,0x8E,0x07,0x51,0x08,0x21,0x08,0x21,0x08,0x51,0x08,0xD1,0x04,0x0E,0x03,0x00,0x00,0x00,0x00, // 8
     0x00,0x00,0x3C,0x00,0x42,0x08,0x81,0x08,0x81,0x08,0x81,0x08,0x42,0x06,0xFC,0x01,0x00,0x00,0x00,0x00, // 9
     0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x0C,0x30,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // :
     0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x4C,0x30,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ;
     0x00,0x00,0x00,0x00,0x80,0x01,0x80,0x01,0x40,0x02,0x40,0x02,0x20,0x04,0x20,0x04,0x10,0x08,0x00,0x00, // <
     0x00,0x00,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x00,0x00, // =
     0x00,0x00,0x10,0x08,0x20,0x04,0x20,0x04,0x40,0x02,0x40,0x02,0x80,0x01,0x80,0x01,0x00,0x00,0x00,0x00, // >
     0x00,0x00,0x07,0x00,0x01,0x00,0x81,0x0D,0x41,0x00,0x21,0x00,0x13,0x00,0x0E,0x00,0x00,0x00,0x00,0x00, // ?
     0xF8,0x01,0x0C,0x06,0x02,0x0C,0xF1,0x09,0x09,0x0A,0x05,0x0B,0xC5,0x0D,0xFE,0x03,0x00,0x02,0x00,0x02, // @
     0x00,0x08,0x00,0x07,0xC0,0x01,0x38,0x01,0x0C,0x01,0x18,0x01,0x60,0x01,0x80,0x01,0x00,0x06,0x00,0x08, // A
     0x00,0x00,0xFC,0x0F,0x44,0x08,0x44,0x08,0x44,0x08,0x44,0x08,0xA4,0x08,0x18,0x07,0x00,0x00,0x00,0x00, // B
     0xE0,0x01,0x18,0x06,0x08,0x04,0x04,0x08,0x04,0x08,0x04,0x08,0x04,0x08,0x0C,0x08,0x00,0x00,0x00,0x00, // C
     0x00,0x00,0xFC,0x0F,0x04,0x08,0x04,0x08,0x04,0x08,0x04,0x08,0x08,0x04,0xF0,0x03,0x00,0x00,0x00,0x00, // D
     0x00,0x00,0xFC,0x0F,0x84,0x08,0x84,0x08,0x84,0x08,0x84,0x08,0x84,0x08,0x04,0x08,0x00,0x00,0x00,0x00, // E
     0x00,0x00,0xFC,0x0F,0x84,0x00,0x84,0x00,0x84,0x00,0x84,0x00,0x84,0x00,0x04,0x00,0x00,0x00,0x00,0x00, // F
     0x00,0x00,0xE0,0x01,0x18,0x06,0x08,0x04,0x04,0x08,0x04,0x08,0x84,0x08,0x84,0x08,0x8C,0x0F,0x00,0x00, // G
     0x00,0x00,0xFC,0x0F,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0xFC,0x0F,0x00,0x00,0x00,0x00, // H
     0x00,0x00,0x04,0x08,0x04,0x08,0x04,0x08,0xFC,0x0F,0x04,0x08,0x04,0x08,0x04,0x08,0x00,0x00,0x00,0x00, // I
     0x00,0x00,0x00,0x00,0x00,0x08,0x04,0x08,0x04,0x08,0x04,0x08,0xFC,0x07,0x00,0x00,0x00,0x00,0x00,0x00, // J
     0x00,0x00,0xFC,0x0F,0x40,0x00,0xC0,0x00,0x20,0x01,0x10,0x02,0x08,0x02,0x04,0x04,0x00,0x08,0x00,0x00, // K
     0x00,0x00,0xFC,0x0F,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x00,0x00,0x00, // L
     0xFC,0x0F,0x1C,0x00,0xF0,0x00,0x80,0x03,0x00,0x03,0xE0,0x00,0x1C,0x00,0xFC,0x0F,0x00,0x00,0x00,0x00, // M
     0x00,0x00,0xFC,0x0F,0x08,0x00,0x30,0x00,0xC0,0x00,0x00,0x03,0x00,0x04,0xFC,0x0F,0x00,0x00,0x00,0x00, // N
     0x00,0x00,0xF0,0x03,0x08,0x04,0x04,0x08,0x04,0x08,0x04,0x08,0x04,0x08,0x08,0x04,0xF0,0x03,0x00,0x00, // O
     0x00,0x00,0xFC,0x0F,0x84,0x00,0x84,0x00,0x84,0x00,0x84,0x00,0x44,0x00,0x38,0x00,0x00,0x00,0x00,0x00, // P
     0x00,0x00,0xF0,0x03,0x08,0x04,0x04,0x08,0x04,0x08,0x04,0x08,0x04,0x18,0x08,0x24,0xF0,0x23,0x00,0x00, // Q
     0x00,0x00,0xFC,0x0F,0x84,0x00,0x84,0x00,0x84,0x01,0x44,0x02,0x38,0x04,0x00,0x08,0x00,0x00,0x00,0x00, // R
     0x00,0x00,0x38,0x0C,0x24,0x08,0x44,0x08,0x44,0x08,0x84,0x08,0x84,0x04,0x0C,0x07,0x00,0x00,0x00,0x00, // S
     0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0xFC,0x0F,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x00,0x00, // T
     0x00,0x00,0xFC,0x03,0x00,0x0C,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x04,0xFC,0x03,0x00,0x00,0x00,0x00, // U
     0x04,0x00,0x18,0x00,0xE0,0x00,0x00,0x03,0x00,0x0C,0x00,0x0C,0x80,0x03,0x60,0x00,0x18,0x00,0x04,0x00, // V
     0x0C,0x00,0xF0,0x01,0x00,0x0E,0x80,0x07,0x70,0x00,0xE0,0x00,0x00,0x07,0x00,0x0E,0xF0,0x01,0x0C,0x00, // W
     0x04,0x08,0x08,0x04,0x10,0x02,0x20,0x01,0xC0,0x00,0xC0,0x00,0x20,0x01,0x10,0x02,0x08,0x04,0x04,0x08, // X
     0x04,0x00,0x08,0x00,0x30,0x00,0x40,0x00,0x80,0x0F,0x40,0x00,0x20,0x00,0x10,0x00,0x08,0x00,0x04,0x00, // Y
     0x00,0x00,0x04,0x0C,0x04,0x0A,0x04,0x09,0x84,0x08,0x44,0x08,0x24,0x08,0x14,0x08,0x0C,0x08,0x00,0x00, // Z
     0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x3F,0x01,0x20,0x01,0x20,0x01,0x20,0x01,0x20,0x00,0x00,0x00,0x00, // [
     0x00,0x00,0x01,0x00,0x06,0x00,0x18,0x00,0x60,0x00,0x80,0x01,0x00,0x06,0x00,0x18,0x00,0x20,0x00,0x00, // "\"
     0x00,0x00,0x01,0x20,0x01,0x20,0x01,0x20,0x01,0x20,0xFF,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ]
     0x00,0x00,0x01,0x20,0x01,0x20,0x01,0x20,0x01,0x20,0xFF,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ] 
     0x00,0x00,0x00,0x02,0x80,0x01,0x70,0x00,0x1C,0x00,0x07,0x00,0x38,0x00,0xC0,0x01,0x00,0x02,0x00,0x00, // ^
     0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10, // _
     //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // `
     0x00,0x00,0x00,0x06,0x10,0x09,0x90,0x08,0x90,0x08,0x90,0x04,0xE0,0x0F,0x00,0x08,0x00,0x00,0x00,0x00, // a
     0x00,0x00,0xFF,0x0F,0x20,0x04,0x10,0x08,0x10,0x08,0x10,0x08,0x30,0x04,0xC0,0x03,0x00,0x00,0x00,0x00, // b
     0x00,0x00,0xC0,0x03,0x20,0x04,0x10,0x08,0x10,0x08,0x10,0x08,0x10,0x08,0x10,0x08,0x00,0x00,0x00,0x00, // c
     0x00,0x00,0xC0,0x03,0x20,0x0C,0x10,0x08,0x10,0x08,0x10,0x08,0x20,0x04,0xFF,0x0F,0x00,0x00,0x00,0x00, // d
     0x00,0x00,0xC0,0x03,0xA0,0x04,0x90,0x08,0x90,0x08,0x90,0x08,0x90,0x08,0xE0,0x08,0x00,0x00,0x00,0x00, // e
     0x00,0x00,0x10,0x00,0x10,0x00,0xFE,0x0F,0x12,0x00,0x11,0x00,0x11,0x00,0x11,0x00,0x11,0x00,0x00,0x00, // f
     0x00,0x00,0xC0,0x03,0x20,0x4C,0x10,0x48,0x10,0x48,0x10,0x48,0x20,0x24,0xF0,0x1F,0x00,0x00,0x00,0x00, // g
     0x00,0x00,0xFF,0x0F,0x40,0x00,0x20,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0xE0,0x0F,0x00,0x00,0x00,0x00, // h
     0x00,0x00,0x10,0x00,0x10,0x00,0x13,0x00,0xF3,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // i
     0x00,0x00,0x00,0x40,0x10,0x40,0x10,0x40,0x13,0x40,0xF3,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // j
     0x00,0x00,0xFF,0x0F,0x80,0x00,0x80,0x01,0x40,0x02,0x20,0x02,0x20,0x04,0x10,0x08,0x00,0x00,0x00,0x00, // k
     0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // l
     0xF0,0x0F,0x20,0x00,0x10,0x00,0x10,0x00,0xE0,0x0F,0x20,0x00,0x10,0x00,0x10,0x00,0xE0,0x0F,0x00,0x00, // m
     0x00,0x00,0xF0,0x0F,0x60,0x00,0x20,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0xE0,0x0F,0x00,0x00,0x00,0x00, // n
     0x00,0x00,0xC0,0x03,0x20,0x04,0x10,0x08,0x10,0x08,0x10,0x08,0x20,0x04,0xC0,0x03,0x00,0x00,0x00,0x00, // o
     0x00,0x00,0xF0,0x7F,0x20,0x04,0x10,0x08,0x10,0x08,0x10,0x08,0x30,0x04,0xC0,0x03,0x00,0x00,0x00,0x00, // p
     0x00,0x00,0xC0,0x03,0x20,0x0C,0x10,0x08,0x10,0x08,0x10,0x08,0x20,0x04,0xF0,0x7F,0x00,0x00,0x00,0x00, // q
     0x00,0x00,0x00,0x00,0xF0,0x0F,0x40,0x00,0x20,0x00,0x10,0x00,0x10,0x00,0x70,0x00,0x00,0x00,0x00,0x00, // r
     0x00,0x00,0x60,0x0C,0x90,0x08,0x90,0x08,0x10,0x09,0x10,0x09,0x10,0x06,0x00,0x00,0x00,0x00,0x00,0x00, // s
     0x00,0x00,0x10,0x00,0x10,0x00,0xFC,0x07,0x10,0x08,0x10,0x08,0x10,0x08,0x10,0x08,0x00,0x00,0x00,0x00, // t
     0x00,0x00,0xF0,0x07,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x04,0xF0,0x0F,0x00,0x00,0x00,0x00,0x00,0x00, // u
     0x10,0x00,0xE0,0x00,0x00,0x03,0x00,0x0C,0x00,0x08,0x00,0x06,0x80,0x01,0x60,0x00,0x10,0x00,0x00,0x00, // v
     0x30,0x00,0xC0,0x03,0x00,0x0C,0x00,0x07,0xE0,0x00,0xC0,0x00,0x00,0x07,0x00,0x0C,0xC0,0x03,0x30,0x00, // w
     0x00,0x00,0x10,0x08,0x20,0x04,0x40,0x02,0x80,0x01,0x80,0x01,0x40,0x02,0x20,0x04,0x10,0x08,0x00,0x00, // x
     0x10,0x40,0x60,0x40,0x80,0x41,0x00,0x23,0x00,0x1C,0x00,0x0C,0x00,0x03,0x80,0x00,0x60,0x00,0x10,0x00, // y
     0x00,0x00,0x10,0x08,0x10,0x0C,0x10,0x0A,0x10,0x09,0x90,0x08,0x50,0x08,0x30,0x08,0x10,0x08,0x00,0x00, // z
     0x00,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x7E,0x1F,0x01,0x20,0x01,0x20,0x01,0x20,0x00,0x00,0x00,0x00, // {
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // |
     0x00,0x00,0x00,0x00,0x01,0x20,0x01,0x20,0x01,0x20,0x7E,0x1F,0x80,0x00,0x80,0x00,0x00,0x00,0x00,0x00, // }
     0x00,0x00,0x00,0x00,0x1E,0x00,0x21,0x00,0x21,0x00,0x21,0x00,0x21,0x00,0x1E,0x00,0x00,0x00,0x00,0x00}; // 
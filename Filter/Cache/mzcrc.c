/*
 Copyright (C) JAEHYUK CHO
 All rights reserved.
 
 Author: JaeHyuk Cho <minzkn@minzkn.com>
*/

#if !defined(__SOURCE_MZCRC_C__)
# define __SOURCE_MZCRC_C__ "mzcrc.c"

#include "mzcrc.h"

def_mz_export_c mz_uint8_t mz_reverse_bit_order_uint8(mz_uint8_t s_value);
def_mz_export_c mz_uint16_t mz_reverse_bit_order_uint16(mz_uint16_t s_value);
def_mz_export_c mz_uint32_t mz_reverse_bit_order_uint32(mz_uint32_t s_value);
def_mz_export_c mz_uint64_t mz_reverse_bit_order_uint64(mz_uint64_t s_value);

def_mz_export_c mz_crc16_t mz_crc16_ibm(int s_is_first, mz_crc16_t s_seed, const void *s_data, size_t s_size);
def_mz_export_c mz_crc16_t mz_crc16_ccitt(int s_is_first, mz_crc16_t s_seed, const void *s_data, size_t s_size);
def_mz_export_c mz_crc32_t mz_crc32_ieee_802_3(int s_is_first, mz_crc32_t s_seed, const void *s_data, size_t s_size);
def_mz_export_c mz_crc64_t mz_crc64_iso(int s_is_first, mz_crc64_t s_seed, const void *s_data, size_t s_size);

static const mz_uint8_t __g_mz_reverse_bit_order_table[ /* 256 */ ] = {
    /* 0x00 */  mz_uint8_const(0x00), mz_uint8_const(0x80), mz_uint8_const(0x40), mz_uint8_const(0xc0),
    /* 0x04 */  mz_uint8_const(0x20), mz_uint8_const(0xa0), mz_uint8_const(0x60), mz_uint8_const(0xe0),
    /* 0x08 */  mz_uint8_const(0x10), mz_uint8_const(0x90), mz_uint8_const(0x50), mz_uint8_const(0xd0),
    /* 0x0c */  mz_uint8_const(0x30), mz_uint8_const(0xb0), mz_uint8_const(0x70), mz_uint8_const(0xf0),
    /* 0x10 */  mz_uint8_const(0x08), mz_uint8_const(0x88), mz_uint8_const(0x48), mz_uint8_const(0xc8),
    /* 0x14 */  mz_uint8_const(0x28), mz_uint8_const(0xa8), mz_uint8_const(0x68), mz_uint8_const(0xe8),
    /* 0x18 */  mz_uint8_const(0x18), mz_uint8_const(0x98), mz_uint8_const(0x58), mz_uint8_const(0xd8),
    /* 0x1c */  mz_uint8_const(0x38), mz_uint8_const(0xb8), mz_uint8_const(0x78), mz_uint8_const(0xf8),
    /* 0x20 */  mz_uint8_const(0x04), mz_uint8_const(0x84), mz_uint8_const(0x44), mz_uint8_const(0xc4),
    /* 0x24 */  mz_uint8_const(0x24), mz_uint8_const(0xa4), mz_uint8_const(0x64), mz_uint8_const(0xe4),
    /* 0x28 */  mz_uint8_const(0x14), mz_uint8_const(0x94), mz_uint8_const(0x54), mz_uint8_const(0xd4),
    /* 0x2c */  mz_uint8_const(0x34), mz_uint8_const(0xb4), mz_uint8_const(0x74), mz_uint8_const(0xf4),
    /* 0x30 */  mz_uint8_const(0x0c), mz_uint8_const(0x8c), mz_uint8_const(0x4c), mz_uint8_const(0xcc),
    /* 0x34 */  mz_uint8_const(0x2c), mz_uint8_const(0xac), mz_uint8_const(0x6c), mz_uint8_const(0xec),
    /* 0x38 */  mz_uint8_const(0x1c), mz_uint8_const(0x9c), mz_uint8_const(0x5c), mz_uint8_const(0xdc),
    /* 0x3c */  mz_uint8_const(0x3c), mz_uint8_const(0xbc), mz_uint8_const(0x7c), mz_uint8_const(0xfc),
    /* 0x40 */  mz_uint8_const(0x02), mz_uint8_const(0x82), mz_uint8_const(0x42), mz_uint8_const(0xc2),
    /* 0x44 */  mz_uint8_const(0x22), mz_uint8_const(0xa2), mz_uint8_const(0x62), mz_uint8_const(0xe2),
    /* 0x48 */  mz_uint8_const(0x12), mz_uint8_const(0x92), mz_uint8_const(0x52), mz_uint8_const(0xd2),
    /* 0x4c */  mz_uint8_const(0x32), mz_uint8_const(0xb2), mz_uint8_const(0x72), mz_uint8_const(0xf2),
    /* 0x50 */  mz_uint8_const(0x0a), mz_uint8_const(0x8a), mz_uint8_const(0x4a), mz_uint8_const(0xca),
    /* 0x54 */  mz_uint8_const(0x2a), mz_uint8_const(0xaa), mz_uint8_const(0x6a), mz_uint8_const(0xea),
    /* 0x58 */  mz_uint8_const(0x1a), mz_uint8_const(0x9a), mz_uint8_const(0x5a), mz_uint8_const(0xda),
    /* 0x5c */  mz_uint8_const(0x3a), mz_uint8_const(0xba), mz_uint8_const(0x7a), mz_uint8_const(0xfa),
    /* 0x60 */  mz_uint8_const(0x06), mz_uint8_const(0x86), mz_uint8_const(0x46), mz_uint8_const(0xc6),
    /* 0x64 */  mz_uint8_const(0x26), mz_uint8_const(0xa6), mz_uint8_const(0x66), mz_uint8_const(0xe6),
    /* 0x68 */  mz_uint8_const(0x16), mz_uint8_const(0x96), mz_uint8_const(0x56), mz_uint8_const(0xd6),
    /* 0x6c */  mz_uint8_const(0x36), mz_uint8_const(0xb6), mz_uint8_const(0x76), mz_uint8_const(0xf6),
    /* 0x70 */  mz_uint8_const(0x0e), mz_uint8_const(0x8e), mz_uint8_const(0x4e), mz_uint8_const(0xce),
    /* 0x74 */  mz_uint8_const(0x2e), mz_uint8_const(0xae), mz_uint8_const(0x6e), mz_uint8_const(0xee),
    /* 0x78 */  mz_uint8_const(0x1e), mz_uint8_const(0x9e), mz_uint8_const(0x5e), mz_uint8_const(0xde),
    /* 0x7c */  mz_uint8_const(0x3e), mz_uint8_const(0xbe), mz_uint8_const(0x7e), mz_uint8_const(0xfe),
    /* 0x80 */  mz_uint8_const(0x01), mz_uint8_const(0x81), mz_uint8_const(0x41), mz_uint8_const(0xc1),
    /* 0x84 */  mz_uint8_const(0x21), mz_uint8_const(0xa1), mz_uint8_const(0x61), mz_uint8_const(0xe1),
    /* 0x88 */  mz_uint8_const(0x11), mz_uint8_const(0x91), mz_uint8_const(0x51), mz_uint8_const(0xd1),
    /* 0x8c */  mz_uint8_const(0x31), mz_uint8_const(0xb1), mz_uint8_const(0x71), mz_uint8_const(0xf1),
    /* 0x90 */  mz_uint8_const(0x09), mz_uint8_const(0x89), mz_uint8_const(0x49), mz_uint8_const(0xc9),
    /* 0x94 */  mz_uint8_const(0x29), mz_uint8_const(0xa9), mz_uint8_const(0x69), mz_uint8_const(0xe9),
    /* 0x98 */  mz_uint8_const(0x19), mz_uint8_const(0x99), mz_uint8_const(0x59), mz_uint8_const(0xd9),
    /* 0x9c */  mz_uint8_const(0x39), mz_uint8_const(0xb9), mz_uint8_const(0x79), mz_uint8_const(0xf9),
    /* 0xa0 */  mz_uint8_const(0x05), mz_uint8_const(0x85), mz_uint8_const(0x45), mz_uint8_const(0xc5),
    /* 0xa4 */  mz_uint8_const(0x25), mz_uint8_const(0xa5), mz_uint8_const(0x65), mz_uint8_const(0xe5),
    /* 0xa8 */  mz_uint8_const(0x15), mz_uint8_const(0x95), mz_uint8_const(0x55), mz_uint8_const(0xd5),
    /* 0xac */  mz_uint8_const(0x35), mz_uint8_const(0xb5), mz_uint8_const(0x75), mz_uint8_const(0xf5),
    /* 0xb0 */  mz_uint8_const(0x0d), mz_uint8_const(0x8d), mz_uint8_const(0x4d), mz_uint8_const(0xcd),
    /* 0xb4 */  mz_uint8_const(0x2d), mz_uint8_const(0xad), mz_uint8_const(0x6d), mz_uint8_const(0xed),
    /* 0xb8 */  mz_uint8_const(0x1d), mz_uint8_const(0x9d), mz_uint8_const(0x5d), mz_uint8_const(0xdd),
    /* 0xbc */  mz_uint8_const(0x3d), mz_uint8_const(0xbd), mz_uint8_const(0x7d), mz_uint8_const(0xfd),
    /* 0xc0 */  mz_uint8_const(0x03), mz_uint8_const(0x83), mz_uint8_const(0x43), mz_uint8_const(0xc3),
    /* 0xc4 */  mz_uint8_const(0x23), mz_uint8_const(0xa3), mz_uint8_const(0x63), mz_uint8_const(0xe3),
    /* 0xc8 */  mz_uint8_const(0x13), mz_uint8_const(0x93), mz_uint8_const(0x53), mz_uint8_const(0xd3),
    /* 0xcc */  mz_uint8_const(0x33), mz_uint8_const(0xb3), mz_uint8_const(0x73), mz_uint8_const(0xf3),
    /* 0xd0 */  mz_uint8_const(0x0b), mz_uint8_const(0x8b), mz_uint8_const(0x4b), mz_uint8_const(0xcb),
    /* 0xd4 */  mz_uint8_const(0x2b), mz_uint8_const(0xab), mz_uint8_const(0x6b), mz_uint8_const(0xeb),
    /* 0xd8 */  mz_uint8_const(0x1b), mz_uint8_const(0x9b), mz_uint8_const(0x5b), mz_uint8_const(0xdb),
    /* 0xdc */  mz_uint8_const(0x3b), mz_uint8_const(0xbb), mz_uint8_const(0x7b), mz_uint8_const(0xfb),
    /* 0xe0 */  mz_uint8_const(0x07), mz_uint8_const(0x87), mz_uint8_const(0x47), mz_uint8_const(0xc7),
    /* 0xe4 */  mz_uint8_const(0x27), mz_uint8_const(0xa7), mz_uint8_const(0x67), mz_uint8_const(0xe7),
    /* 0xe8 */  mz_uint8_const(0x17), mz_uint8_const(0x97), mz_uint8_const(0x57), mz_uint8_const(0xd7),
    /* 0xec */  mz_uint8_const(0x37), mz_uint8_const(0xb7), mz_uint8_const(0x77), mz_uint8_const(0xf7),
    /* 0xf0 */  mz_uint8_const(0x0f), mz_uint8_const(0x8f), mz_uint8_const(0x4f), mz_uint8_const(0xcf),
    /* 0xf4 */  mz_uint8_const(0x2f), mz_uint8_const(0xaf), mz_uint8_const(0x6f), mz_uint8_const(0xef),
    /* 0xf8 */  mz_uint8_const(0x1f), mz_uint8_const(0x9f), mz_uint8_const(0x5f), mz_uint8_const(0xdf),
    /* 0xfc */  mz_uint8_const(0x3f), mz_uint8_const(0xbf), mz_uint8_const(0x7f), mz_uint8_const(0xff)
};
#define __mz_reverse_bit_order_uint8(m_value) __g_mz_reverse_bit_order_table[m_value]

mz_uint8_t mz_reverse_bit_order_uint8(mz_uint8_t s_value)
{
    return(__mz_reverse_bit_order_uint8(s_value));
}

mz_uint16_t mz_reverse_bit_order_uint16(mz_uint16_t s_value)
{
    mz_uint16_t s_result;

    mz_poke_uint8(&s_result, 1, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 0)));
    mz_poke_uint8(&s_result, 0, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 1)));

    return(s_result);
}

mz_uint32_t mz_reverse_bit_order_uint32(mz_uint32_t s_value)
{
    mz_uint32_t s_result;
    
    mz_poke_uint8(&s_result, 3, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 0)));
    mz_poke_uint8(&s_result, 2, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 1)));
    mz_poke_uint8(&s_result, 1, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 2)));
    mz_poke_uint8(&s_result, 0, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 3)));

    return(s_result);
}

mz_uint64_t mz_reverse_bit_order_uint64(mz_uint64_t s_value)
{
    mz_uint64_t s_result;
    
    mz_poke_uint8(&s_result, 7, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 0)));
    mz_poke_uint8(&s_result, 6, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 1)));
    mz_poke_uint8(&s_result, 5, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 2)));
    mz_poke_uint8(&s_result, 4, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 3)));
    mz_poke_uint8(&s_result, 3, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 4)));
    mz_poke_uint8(&s_result, 2, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 5)));
    mz_poke_uint8(&s_result, 1, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 6)));
    mz_poke_uint8(&s_result, 0, __mz_reverse_bit_order_uint8(mz_peek_uint8(&s_value, 7)));

    return(s_result);
}

mz_crc16_t mz_crc16_ibm(int s_is_first, mz_crc16_t s_seed, const void *s_data, size_t s_size)
{
    __mz_crc_slow_template(crc16,def_mz_crc16_ibm_polynomial,def_mz_crc16_ibm_final_xor,s_is_first,s_seed,s_data,s_size);
}

mz_crc16_t mz_crc16_ccitt(int s_is_first, mz_crc16_t s_seed, const void *s_data, size_t s_size)
{
    __mz_crc_slow_template(crc16,def_mz_crc16_ccitt_polynomial,def_mz_crc16_ccitt_final_xor,s_is_first,s_seed,s_data,s_size);
}

mz_crc32_t mz_crc32_ieee_802_3(int s_is_first, mz_crc32_t s_seed, const void *s_data, size_t s_size)
{
    __mz_crc_fast_template(crc32,def_mz_crc32_ieee_802_3_polynomial,def_mz_crc32_ieee_802_3_final_xor,s_is_first,s_seed,s_data,s_size);
}

mz_crc64_t mz_crc64_iso(int s_is_first, mz_crc64_t s_seed, const void *s_data, size_t s_size)
{
    __mz_crc_slow_template(crc64,def_mz_crc64_iso_polynomial,def_mz_crc64_iso_final_xor,s_is_first,s_seed,s_data,s_size);
}

#endif

/* vim: set expandtab: */
/* End of source */

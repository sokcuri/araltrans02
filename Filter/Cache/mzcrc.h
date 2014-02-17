/*
 Copyright (C) JAEHYUK CHO
 All rights reserved.
 
 Author: JaeHyuk Cho <minzkn@minzkn.com>
*/

#if !defined(__HEADER_MZCRC_H__)
#define __HEADER_MZCRC_H__ "mzcrc.h"

#include <stdlib.h>

/* ---- C/C++ export/import macro */

#if defined(__cplusplus)
# if !defined(def_mz_import_c)
#  define def_mz_import_c                                            extern "C"
# endif
# if !defined(def_mz_export_c)
#  define def_mz_export_c                                            extern "C"
# endif
#else
# if !defined(def_mz_import_c)
#  define def_mz_import_c                                            extern
# endif
# if !defined(def_mz_export_c)
#  define def_mz_export_c
# endif
#endif

/* ---- Built-in expect macro */

#if defined(__GNUC__)
# define mz_builtin_expect(m_expression,m_value)                     __builtin_expect((long)(m_expression),(long)(m_value))
#else
# define mz_builtin_expect(m_expression,m_value)                     m_expression
#endif

/* ---- System type define */

#if !defined(mz_uint8_t)
# define __mz_uint8_t                                                unsigned char
# define mz_uint8_t                                                  __mz_uint8_t
# define mz_uint8_const(m_value)                                     ((__mz_uint8_t)(m_value##u))
#endif

#if !defined(mz_uint16_t)
# define __mz_uint16_t                                               unsigned short
# define mz_uint16_t                                                 __mz_uint16_t
# define mz_uint16_const(m_value)                                    ((__mz_uint16_t)(m_value##u))
#endif

#if !defined(mz_uint32_t)
# define __mz_uint32_t                                               unsigned int
# define mz_uint32_t                                                 __mz_uint32_t
# define mz_uint32_const(m_value)                                    ((__mz_uint32_t)(m_value##u))
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_MSC_VER)
# if !defined(mz_uint64_t)
#  define __mz_uint64_t                                              unsigned __int64
#  define mz_uint64_t                                                __mz_uint64_t
#  define mz_uint64_const(m_value)                                   ((__mz_uint64_t)(m_value##ui64))
# endif
#else
# if !defined(mz_uint64_t)
#  define __mz_uint64_t                                              unsigned long long
#  define mz_uint64_t                                                __mz_uint64_t
#  define mz_uint64_const(m_value)                                   ((__mz_uint64_t)(m_value##ull))
# endif
#endif

/* ---- CRC type define */

#if !defined(mz_crc8_t)
# define __mz_crc8_t                                                 __mz_uint8_t
# define mz_crc8_t                                                   __mz_crc8_t
# define mz_crc8_const(m_value)                                      mz_uint8_const(m_value)
#endif

#if !defined(mz_crc16_t)
# define __mz_crc16_t                                                __mz_uint16_t
# define mz_crc16_t                                                  __mz_crc16_t
# define mz_crc16_const(m_value)                                     mz_uint16_const(m_value)
#endif

#if !defined(mz_crc32_t)
# define __mz_crc32_t                                                __mz_uint32_t
# define mz_crc32_t                                                  __mz_crc32_t
# define mz_crc32_const(m_value)                                     mz_uint32_const(m_value)
#endif

#if !defined(mz_crc64_t)
# define __mz_crc64_t                                                __mz_uint64_t
# define mz_crc64_t                                                  __mz_crc64_t
# define mz_crc64_const(m_value)                                     mz_uint64_const(m_value)
#endif

/* ---- Const pointer macro */

#if !defined(mz_peek_const_vector)
# define mz_peek_const_vector(m_cast,m_base,m_sign,m_offset)         ((m_cast)((const void *)(((const mz_uint8_t *)(m_base)) m_sign ((size_t)(m_offset)))))
#endif

#if !defined(mz_peek_const_f)
# define mz_peek_const_f(m_cast,m_base,m_offset)                     mz_peek_const_vector(m_cast,m_base,+,m_offset)
#endif

#if !defined(mz_peek_const_r)
# define mz_peek_const_r(m_cast,m_base,m_offset)                     mz_peek_const_vector(m_cast,m_base,-,m_offset)
#endif

#if !defined(mz_peek_const)
# define mz_peek_const(m_base,m_offset)                              mz_peek_const_vector(const void *,m_base,+,m_offset)
#endif

/* ---- Pointer macro */

#if !defined(mz_peek_vector)
# define mz_peek_vector(m_cast,m_base,m_sign,m_offset)               ((m_cast)((void *)(((mz_uint8_t *)(m_base)) m_sign ((size_t)(m_offset)))))
#endif

#if !defined(mz_peek_f)
# define mz_peek_f(m_cast,m_base,m_offset)                           mz_peek_vector(m_cast,m_base,+,m_offset)
#endif

#if !defined(mz_peek_r)
# define mz_peek_r(m_cast,m_base,m_offset)                           mz_peek_vector(m_cast,m_base,-,m_offset)
#endif

#if !defined(mz_peek)
# define mz_peek(m_base,m_offset)                                    mz_peek_vector(void *,m_base,+,m_offset)
#endif

/* ---- Memory peek macro */

#if !defined(mz_peek_const_type)
# define mz_peek_const_type(m_cast,m_from,m_offset)                  (*(mz_peek_const_f(const m_cast *,m_from,m_offset)))
#endif

#if !defined(mz_peek_uint8)
# define mz_peek_uint8(m_from,m_offset)                              mz_peek_const_type(mz_uint8_t,m_from,m_offset)
#endif

#if !defined(mz_peek_uint16)
# define mz_peek_uint16(m_from,m_offset)                             mz_peek_const_type(mz_uint16_t,m_from,m_offset)
#endif

#if !defined(mz_peek_uint32)
# define mz_peek_uint32(m_from,m_offset)                             mz_peek_const_type(mz_uint32_t,m_from,m_offset)
#endif

#if !defined(mz_peek_uint64)
# define mz_peek_uint64(m_from,m_offset)                             mz_peek_const_type(mz_uint64_t,m_from,m_offset)
#endif

/* ---- Memory poke macro */

#if !defined(mz_peek_type)
# define mz_peek_type(m_cast,m_from,m_offset)                        (*(mz_peek_f(m_cast *,m_from,m_offset)))
#endif

#if !defined(mz_poke_type)
# define mz_poke_type(m_cast,m_from,m_offset,m_value)                do{mz_peek_type(m_cast,m_from,m_offset)=(m_cast)(m_value);}while(0)
#endif

#if !defined(mz_poke_uint8)
# define mz_poke_uint8(m_from,m_offset,m_value)                      mz_poke_type(mz_uint8_t,m_from,m_offset,m_value)
#endif

#if !defined(mz_poke_uint16)
# define mz_poke_uint16(m_from,m_offset,m_value)                     mz_poke_type(mz_uint16_t,m_from,m_offset,m_value)
#endif

#if !defined(mz_poke_uint32)
# define mz_poke_uint32(m_from,m_offset,m_value)                     mz_poke_type(mz_uint32_t,m_from,m_offset,m_value)
#endif

#if !defined(mz_poke_uint64)
# define mz_poke_uint64(m_from,m_offset,m_value)                     mz_poke_type(mz_uint64_t,m_from,m_offset,m_value)
#endif

/* ---- Lock/Unlock wrapper macro */

#if !defined(mz_lock_t)
# define __mz_lock_t                                                 int
# define mz_lock_t                                                   __mz_lock_t
#endif

#if !defined(def_mz_init_lock)
# define def_mz_init_lock                                            0
#endif

#if !defined(mz_rw_lock)
# define mz_rw_lock(m_lock)                                          *(m_lock)=1
#endif

#if !defined(mz_unlock)
# define mz_unlock(m_lock)                                           *(m_lock)=0
#endif

/* ---- CRC const define */

#define def_mz_crc16_ibm_polynomial                                  mz_crc16_const(0x8005)
#define def_mz_crc16_ibm_final_xor                                   mz_crc16_const(0x0000)
#define def_mz_crc16_ibm_init                                        mz_crc16_const(0x0000)

#define def_mz_crc16_ccitt_polynomial                                mz_crc16_const(0x1021)
#define def_mz_crc16_ccitt_final_xor                                 mz_crc16_const(0x0000)
#define def_mz_crc16_ccitt_init                                      mz_crc16_const(0xffff)

#define def_mz_crc32_ieee_802_3_polynomial                           mz_crc32_const(0x04c11db7)
#define def_mz_crc32_ieee_802_3_final_xor                            mz_crc32_const(0xffffffff)
#define def_mz_crc32_ieee_802_3_init                                 mz_crc32_const(0xffffffff)

#define def_mz_crc64_iso_polynomial                                  mz_crc64_const(0x000000000000001b)
#define def_mz_crc64_iso_final_xor                                   mz_crc64_const(0xffffffffffffffff)
#define def_mz_crc64_iso_init                                        mz_crc64_const(0xffffffffffffffff)

/* ---- CRC logic template macro */

#define mz_reverse_bit_order_crc8(m_value)                           mz_reverse_bit_order_uint8(m_value)
#define mz_reverse_bit_order_crc16(m_value)                          mz_reverse_bit_order_uint16(m_value)
#define mz_reverse_bit_order_crc32(m_value)                          mz_reverse_bit_order_uint32(m_value)
#define mz_reverse_bit_order_crc64(m_value)                          mz_reverse_bit_order_uint64(m_value)

/* CRC(Cyclic Redundancy Check) 8/16/32/64/... template (big, fast) */
#define __mz_crc_fast_template(m_type,m_polynomial,m_final_xor,m_is_first,m_seed,m_data,m_size) \
    static mz_lock_t sg_lock = def_mz_init_lock;\
    static volatile int sg_init_table = 0;\
    static volatile mz_##m_type##_t sg_table[ 256 ];\
\
    size_t s_index;\
\
    if(mz_builtin_expect((m_data) == NULL, 0)) {\
        return((m_seed));\
    }\
\
    mz_rw_lock((mz_lock_t *)(&sg_lock));\
    if(mz_builtin_expect(sg_init_table == 0, 0)) {\
        mz_##m_type##_t s_reverse_polynomial = mz_reverse_bit_order_##m_type(m_polynomial);\
        mz_##m_type##_t s_value;\
\
        for(s_index = ((size_t)0);s_index < ((sizeof(sg_table) / sizeof(mz_##m_type##_t)));s_index++) {\
            s_value = (mz_##m_type##_t)s_index;\
            s_value = (s_value >> 1) ^ (s_reverse_polynomial & ((~(s_value & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
            s_value = (s_value >> 1) ^ (s_reverse_polynomial & ((~(s_value & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
            s_value = (s_value >> 1) ^ (s_reverse_polynomial & ((~(s_value & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
            s_value = (s_value >> 1) ^ (s_reverse_polynomial & ((~(s_value & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
            s_value = (s_value >> 1) ^ (s_reverse_polynomial & ((~(s_value & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
            s_value = (s_value >> 1) ^ (s_reverse_polynomial & ((~(s_value & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
            s_value = (s_value >> 1) ^ (s_reverse_polynomial & ((~(s_value & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
            s_value = (s_value >> 1) ^ (s_reverse_polynomial & ((~(s_value & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
            sg_table[s_index] = s_value;\
        }\
\
        sg_init_table = 1;\
    }\
    mz_unlock((mz_lock_t *)(&sg_lock));\
\
    if((m_is_first) == 0) {\
        (m_seed) ^= (m_final_xor);\
    }\
\
    for(s_index = ((size_t)0);s_index < (m_size);s_index++) {\
        (m_seed) = sg_table[((m_seed) ^ ((mz_##m_type##_t)mz_peek_uint8((m_data), s_index))) & mz_##m_type##_const(0xff)] ^ ((m_seed) >> 8);\
    }\
\
    return((m_seed) ^ (m_final_xor))

/* CRC(Cyclic Redundancy Check) 8/16/32/64/... slow template (small, slow) */
#define __mz_crc_slow_template(m_type,m_polynomial,m_final_xor,m_is_first,m_seed,m_data,m_size) \
    size_t s_offset;\
    mz_##m_type##_t s_reverse_polynomial = mz_reverse_bit_order_##m_type(m_polynomial);\
\
    if(mz_builtin_expect((m_data) == NULL, 0)) {\
        return((m_seed));\
    }\
\
    if((m_is_first) == 0) {\
        (m_seed) ^= (m_final_xor);\
    }\
\
    for(s_offset = ((size_t)0);s_offset < (m_size);s_offset++) {\
        (m_seed) = (m_seed) ^ ((mz_##m_type##_t)mz_peek_uint8((m_data),s_offset));\
        (m_seed) = ((m_seed) >> 1) ^ (s_reverse_polynomial & ((~((m_seed) & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
        (m_seed) = ((m_seed) >> 1) ^ (s_reverse_polynomial & ((~((m_seed) & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
        (m_seed) = ((m_seed) >> 1) ^ (s_reverse_polynomial & ((~((m_seed) & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
        (m_seed) = ((m_seed) >> 1) ^ (s_reverse_polynomial & ((~((m_seed) & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
        (m_seed) = ((m_seed) >> 1) ^ (s_reverse_polynomial & ((~((m_seed) & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
        (m_seed) = ((m_seed) >> 1) ^ (s_reverse_polynomial & ((~((m_seed) & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
        (m_seed) = ((m_seed) >> 1) ^ (s_reverse_polynomial & ((~((m_seed) & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
        (m_seed) = ((m_seed) >> 1) ^ (s_reverse_polynomial & ((~((m_seed) & mz_##m_type##_const(1))) + mz_##m_type##_const(1)));\
    }\
\
    return(s_seed ^ (m_final_xor))

/* ---- crc api */

#if !defined(__SOURCE_MZCRC_C__)

def_mz_import_c mz_uint8_t mz_reverse_bit_order_uint8(mz_uint8_t s_value);
def_mz_import_c mz_uint16_t mz_reverse_bit_order_uint16(mz_uint16_t s_value);
def_mz_import_c mz_uint32_t mz_reverse_bit_order_uint32(mz_uint32_t s_value);
def_mz_import_c mz_uint64_t mz_reverse_bit_order_uint64(mz_uint64_t s_value);

def_mz_import_c mz_crc16_t mz_crc16_ibm(int s_is_first, mz_crc16_t s_seed, const void *s_data, size_t s_size);
def_mz_import_c mz_crc16_t mz_crc16_ccitt(int s_is_first, mz_crc16_t s_seed, const void *s_data, size_t s_size);
def_mz_import_c mz_crc32_t mz_crc32_ieee_802_3(int s_is_first, mz_crc32_t s_seed, const void *s_data, size_t s_size);
def_mz_import_c mz_crc64_t mz_crc64_iso(int s_is_first, mz_crc64_t s_seed, const void *s_data, size_t s_size);

#endif

/* ---- default logic select */

/* Generic CRC16 is IBM */
#define def_mz_crc16_polynomial                                      def_mz_crc16_ibm_polynomial
#define def_mz_crc16_final_xor                                       def_mz_crc16_ibm_final_xor
#define def_mz_crc16_init                                            def_mz_crc16_ibm_init
#define mz_crc16(m_is_first,m_seed,m_data,m_size)                    mz_crc16_ibm(m_is_first,m_seed,m_data,m_size)
#define mz_crc16_first(m_data,m_size)                                mz_crc16(def_mz_true,def_mz_crc16_init,m_data,m_size)
#define mz_crc16_update(m_seed,m_data,m_size)                        mz_crc16(def_mz_false,m_seed,m_data,m_size)

/* Generic CRC32 is IEEE 802.3 */
#define def_mz_crc32_polynomial                                      def_mz_crc32_ieee_802_3_polynomial
#define def_mz_crc32_final_xor                                       def_mz_crc32_ieee_802_3_final_xor
#define def_mz_crc32_init                                            def_mz_crc32_ieee_802_3_init
#define mz_crc32(m_is_first,m_seed,m_data,m_size)                    mz_crc32_ieee_802_3(m_is_first,m_seed,m_data,m_size)
#define mz_crc32_first(m_data,m_size)                                mz_crc32(def_mz_true,def_mz_crc32_init,m_data,m_size)
#define mz_crc32_update(m_seed,m_data,m_size)                        mz_crc32(def_mz_false,m_seed,m_data,m_size)

/* Generic CRC64 is ISO */
#define def_mz_crc64_polynomial                                      def_mz_crc64_iso_polynomial
#define def_mz_crc64_final_xor                                       def_mz_crc64_iso_final_xor
#define def_mz_crc64_init                                            def_mz_crc64_iso_init
#define mz_crc64(m_is_first,m_seed,m_data,m_size)                    mz_crc64_iso(m_is_first,m_seed,m_data,m_size)
#define mz_crc64_first(m_data,m_size)                                mz_crc64(def_mz_true,def_mz_crc64_init,m_data,m_size)
#define mz_crc64_update(m_seed,m_data,m_size)                        mz_crc64(def_mz_false,m_seed,m_data,m_size)

/* ---- */

#endif

/* vim: set expandtab: */
/* End of source */

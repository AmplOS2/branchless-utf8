#pragma once
#include <stdint.h>

namespace {

/**
 * Decode the next character, `c`, from `buf`, reporting errors in `e`.
 *
 * Since this is a branchless decoder, four bytes will be read from the
 * buffer regardless of the actual length of the next character. This
 * means the buffer _must_ not crash your program in that instance.
 *
 * Errors are reported in `e`, which will be non-zero if the parsed
 * character was somehow invalid: invalid byte sequence, non-canonical
 * encoding, or a surrogate half.
 *
 * The function returns a pointer to the next character. When an error
 * occurs, this pointer will be a guess that depends on the particular
 * error, but it will always advance at least one byte.
 */
constexpr inline static uint8_t *utf8_decode(uint8_t  *buf,
                                             uint32_t &c,
                                             int      &e) {
        const char lengths[]  = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0};
        const int  masks[]    = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
        const uint32_t mins[] = {4194304, 0, 128, 2048, 65536};
        const int      shiftc[] = {0, 18, 12, 6, 0};
        const int      shifte[] = {0, 6, 4, 2, 0};

        uint8_t *s   = buf;
        int      len = lengths[s[0] >> 3];

        // computes the next character early,
        // which improves performance by about 1-2%
        // (Apple clang 12.0.5 on an M1)
        uint8_t *next = s + len + !len;

        /* Assume a four-byte character and load four bytes. Unused bits are
         * shifted out.
         */
        c  = (uint32_t)(s[0] & masks[len]) << 18;
        c |= (uint32_t)(s[1] & 0x3f) << 12;
        c |= (uint32_t)(s[2] & 0x3f) << 6;
        c |= (uint32_t)(s[3] & 0x3f) << 0;
        c >>= shiftc[len];


        /* Accumulate the various error conditions. */
        e  = (c < mins[len]) << 6; // non-canonical encoding
        e |= ((c >> 11) == 0x1b) << 7;  // surrogate half?
        e |= (c > 0x10FFFF) << 8;  // out of range?
        e |= (s[1] & 0xc0) >> 2;
        e |= (s[2] & 0xc0) >> 4;
        e |= (s[3]       ) >> 6;
        e ^= 0x2a; // top two bits of each tail byte correct?
        e >>= shifte[len];

        return next;
}
}

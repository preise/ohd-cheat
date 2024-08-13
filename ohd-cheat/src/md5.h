#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

class MD5 {
public:
    MD5() { init(); }

    void update(const unsigned char* input, size_t length);

    std::string digest();

private:
    void init();

    void transform(const unsigned char block[64]);

    void encode(unsigned char* output, const uint32_t* input, size_t length);

    void decode(uint32_t* output, const unsigned char* input, size_t length);

    static constexpr unsigned char PADDING[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    static constexpr uint32_t S11 = 7;
    static constexpr uint32_t S12 = 12;
    static constexpr uint32_t S13 = 17;
    static constexpr uint32_t S14 = 22;
    static constexpr uint32_t S21 = 5;
    static constexpr uint32_t S22 = 9;
    static constexpr uint32_t S23 = 14;
    static constexpr uint32_t S24 = 20;
    static constexpr uint32_t S31 = 4;
    static constexpr uint32_t S32 = 11;
    static constexpr uint32_t S33 = 16;
    static constexpr uint32_t S34 = 23;
    static constexpr uint32_t S41 = 6;
    static constexpr uint32_t S42 = 10;
    static constexpr uint32_t S43 = 15;
    static constexpr uint32_t S44 = 21;

    static constexpr void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
        a += F(b, c, d) + x + ac;
        a = rotate_left(a, s) + b;
    }

    static constexpr void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
        a += G(b, c, d) + x + ac;
        a = rotate_left(a, s) + b;
    }

    static constexpr void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
        a += H(b, c, d) + x + ac;
        a = rotate_left(a, s) + b;
    }

    static constexpr void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
        a += I(b, c, d) + x + ac;
        a = rotate_left(a, s) + b;
    }

    static constexpr uint32_t F(uint32_t x, uint32_t y, uint32_t z) { return (x & y) | (~x & z); }
    static constexpr uint32_t G(uint32_t x, uint32_t y, uint32_t z) { return (x & z) | (y & ~z); }
    static constexpr uint32_t H(uint32_t x, uint32_t y, uint32_t z) { return x ^ y ^ z; }
    static constexpr uint32_t I(uint32_t x, uint32_t y, uint32_t z) { return y ^ (x | ~z); }

    static constexpr uint32_t rotate_left(uint32_t x, uint32_t n) { return (x << n) | (x >> (32 - n)); }

    uint32_t state[4];
    uint32_t count[2];
    unsigned char buffer[64];
};

std::string md5(std::ifstream& file);
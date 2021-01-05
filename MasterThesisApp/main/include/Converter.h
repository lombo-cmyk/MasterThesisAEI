//
// Created by lukaszk on 03.01.2021.
//

#ifndef MASTERTHESISAPP_CONVERTER_H
#define MASTERTHESISAPP_CONVERTER_H
#include <bitset>

inline std::bitset<8> ConvertToBitset(std::uint8_t byte) {
    return static_cast<std::bitset<8>>(byte);
}

inline std::uint8_t ConvertToUint8(std::bitset<8> byte) {
    return static_cast<uint8_t>(byte.to_ulong());
}

template<std::size_t B>
inline long ConvertToLong(const std::bitset<B>& b) {
    //© 1997-2005 Bit Twiddling Hacks Sean Eron Anderson
    struct {
        long x : B;
    } s;
    return s.x = b.to_ulong();
}

#endif // MASTERTHESISAPP_CONVERTER_H

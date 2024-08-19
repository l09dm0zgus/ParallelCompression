//
// Created by cx9ps3 on 18.08.2024.
//

#pragma once
#include "BitBuffer.hpp"
#include "LZ77.hpp"
#include <cstdint>
#include <optional>

namespace deflate
{
    class BitBuffer;
    class StaticHuffmanDecoder
    {
    private:
        BitBuffer bitBuffer;
        std::uint16_t code{0};
        std::uint32_t extraBits{0};
        std::uint8_t codeBitPosition{0};
        bool isNextDistance = false;
        inline void resetCode();
        std::optional<std::uint16_t> tryDecodeLength();
        std::optional<std::uint16_t> tryDecodeDistance();
        std::optional<std::byte> tryDecodeLiteral();

    public:
        explicit StaticHuffmanDecoder(const BitBuffer &newBitBuffer);
        std::vector<LZ77::Match> decodeData();
    };
}// namespace deflate

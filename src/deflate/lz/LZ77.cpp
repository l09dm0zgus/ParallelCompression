//
// Created by cx9ps3 on 01.08.2024.
//

#include "LZ77.hpp"

deflate::LZ77::Match deflate::LZ77::findBestMatch(const std::vector<std::byte> &data, std::uint16_t position)
{
    Match bestMatch{std::byte{0x0}, 0, 0};

    auto searchLimit = std::min(WINDOW_SIZE, position);
    auto lookaheadBuffer = _mm256_loadu_si256(std::bit_cast<__m256i *>(&data[position]));

    for (std::uint16_t i = 1; i <= searchLimit; i++)
    {
        auto candidate = _mm256_loadu_si256(std::bit_cast<__m256i *>(&data[position - i]));
        auto cmp = _mm256_cmpeq_epi8(lookaheadBuffer, candidate);

        auto mask = _mm256_movemask_epi8(cmp);
        int matchLength = 0;
        while (mask & 1)
        {
            matchLength++;
            mask >>= 1;
        }

        if (matchLength > bestMatch.length && matchLength > 1)
        {
            bestMatch.length = static_cast<std::uint16_t>(matchLength);
            bestMatch.distance = i;
        }
    }
    if (bestMatch.length == 0)
    {
        bestMatch.literal = data[position];
    }

    return bestMatch;
}

std::vector<deflate::LZ77::Match> deflate::LZ77::compress(const std::vector<std::byte> &dataToCompress)
{
    std::vector<Match> compressedData;
    compressedData.reserve(WINDOW_SIZE);

    std::uint16_t position{0};
    auto dataSize = dataToCompress.size();

    while (position < dataSize)
    {
        auto match = findBestMatch(dataToCompress, position);
        if (match.length > 0)
        {
            compressedData.push_back(match);
            position += match.length;
        }
        else
        {
            match.length = 1;
            match.distance = 0;
            compressedData.push_back(match);
            position++;
        }
    }

    return compressedData;
}

std::vector<std::byte> deflate::LZ77::decompress(const std::vector<Match> &compressedData)
{
    std::vector<std::byte> decompressedData;
    decompressedData.reserve(compressedData.size() * 2);

    for (const Match &match: compressedData)
    {
        if (match.length == 1)
        {
            decompressedData.push_back(match.literal);
        }
        else
        {
            auto offset = decompressedData.size() - match.distance;
            for (std::uint16_t i = 0; i < match.length; ++i)
            {
                decompressedData.push_back(decompressedData[offset + i]);
            }
        }
    }

    return decompressedData;
}
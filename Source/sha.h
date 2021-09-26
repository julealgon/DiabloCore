/**
 * @file sha.cpp
 *
 * Interface of functionality for calculating X-SHA-1 (a flawed implementation of SHA-1).
 */
#pragma once

#include <cstddef>
#include <cstdint>

namespace devilution {

constexpr size_t BlockSize = 64;
constexpr size_t SHA1HashSize = 20;

void SHA1Clear();
void SHA1Result(int n, std::byte messageDigest[SHA1HashSize]);
void SHA1Calculate(int n, const std::byte data[BlockSize], std::byte messageDigest[SHA1HashSize]);
void SHA1Reset(int n);

} // namespace devilution

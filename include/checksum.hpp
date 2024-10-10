#pragma once

#include "global.hpp"

/**
 * Checksum Class
 */
class Checksum {

public:
  static std::string computeHash(Ring r, Block b);

#ifdef NAIVE8
  // Naive 8-bit checksum
  static uint8_t naive8(const uint8_t *data);
#endif

#ifdef FLETCHER16
  // Fletcher 16-bit checksum
  static uint16_t fletcher16(const uint8_t *data, size_t len);
#endif
  
#ifdef FLETCHER32
  // Fletcher 32-bit checksum
  static uint32_t fletcher32(const uint16_t *data, size_t len);
#endif

#ifdef FLETCHER64
  // TODO: Fletcher 64-bit checksum
  static uint64_t fletcher64(const uint32_t *data, size_t len);
#endif

#ifdef DJB2
  // TODO: djb2 - dan bernstein hashing function
  static uint32_t djb2(const uint8_t *data);
#endif

#ifdef BLAKE2
  // TODO: Blake2
#endif
};

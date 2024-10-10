#include "main.hpp"

// Compute hash of ring and block, converting into hexidecimal
std::string Checksum::computeHash(Ring r, Block b) {
  std::stringstream ss;
  std::string chksum;

  // Prefix hex string
  ss << r.version;
  ss << r.mode;
  ss << r.algorithm;
  ss << r.difficulty;
  
  // Block content hex string
  ss << Tool::stripHyphens(b.ring_uuid);
  ss << Tool::stripHyphens(b.uuid);
  ss << b.prev_hash;
  ss << Tool::sha256(b.data);
  ss << Tool::dateToEpochHex(b.created);
  ss << b.nonce;

  std::string input(ss.str());
  if (Global::Debug) std::cout << input << std::endl;

#ifdef NAIVE8
  const uint8_t *b8 = reinterpret_cast<const uint8_t *>(input.c_str());
  uint8_t chksum8 = naive8(b8);
  chksum = Tool::decToHex<uint8_t>(chksum8);
#endif

#ifdef FLETCHER16
  const uint8_t *b8 = reinterpret_cast<const uint8_t *>(input.c_str());
  uint16_t chksum16 = fletcher16(b8, strlen(input.c_str()));
  chksum = Tool::decToHex<uint16_t>(chksum16);
#endif

#ifdef FLETCHER32
  const uint16_t *b16 = reinterpret_cast<const uint16_t *>(input.c_str());
  uint32_t chksum32 = fletcher32(b16, strlen(input.c_str())/2);
  chksum = Tool::decToHex<uint32_t>(chksum32);
#endif

#ifdef DJB2
  const uint8_t *b8 = reinterpret_cast<const uint8_t *>(input.c_str());
  uint32_t chksum32 = djb2(b8);
  chksum = Tool::decToHex<uint32_t>(chksum8);
#endif

  if (Global::Debug) std::cout << chksum << std::endl;
  return chksum;
}

#ifdef NAIVE8
// Naive 8-bit checksum
uint8_t Checksum::naive8(const uint8_t *data) {
  uint32_t hash = 0;
  uint8_t d;

  while (d = *data++) {
    hash += d;
  }
  return hash % 0xFF;
}
#endif

#ifdef FLETCHER16
// Fletcher 16-bit checksum
uint16_t Checksum::fletcher16(const uint8_t *data, size_t len) {
  uint32_t c0, c1;
  len = (len + 1) & ~1; // Round up len to words

  // We similarly solve for n > 0 and n * (n+1) / 2 * (2^16-1) < (2^32-1) here.
  // On modern computers, using a 64-bit c0/c1 could allow a group size of 23726746.
  for (c0 = c1 = 0; len > 0; ) {
    size_t blocklen = len;
    if (blocklen > 5002) {
      blocklen = 5002;
    }
    len -= blocklen;
    do {
      c0 = c0 + *data++;
      c1 = c1 + c0;
    } while (--blocklen);
    c0 = c0 % 0xFF;
    c1 = c1 % 0xFF;
  }
  return (c1 << 8 | c0);
}
#endif
  
#ifdef FLETCHER32
// Fletcher 32-bit checksum
uint32_t Checksum::fletcher32(const uint16_t *data, size_t len) {
  uint32_t c0, c1;
  len = (len + 1) & ~1; // Round up len to words

  // We similarly solve for n > 0 and n * (n+1) / 2 * (2^16-1) < (2^32-1) here.
  // On modern computers, using a 64-bit c0/c1 could allow a group size of 23726746.
  for (c0 = c1 = 0; len > 0; ) {
    size_t blocklen = len;
    if (blocklen > 360*2) {
      blocklen = 360*2;
    }
    len -= blocklen;
    do {
      c0 = c0 + *data++;
      c1 = c1 + c0;
    } while ((blocklen -= 2));
    c0 = c0 % 0xFFFF;
    c1 = c1 % 0xFFFF;
  }
  return (c1 << 16 | c0);
}
#endif

#ifdef FLETCHER64
// Fletcher 64-bit checksum
uint64_t Checksum::fletcher64(const uint32_t *data, size_t len) {
  uint64_t c0, c1;
  len = (len + 1) & ~1; // Round up len to words

  // We similarly solve for n > 0 and n * (n+1) / 2 * (2^16-1) < (2^32-1) here.
  // On modern computers, using a 64-bit c0/c1 could allow a group size of 23726746.
  for (c0 = c1 = 0; len > 0; ) {
    size_t blocklen = len;
    if (blocklen > 92681*2) {
      blocklen = 92681*2;
    }
    len -= blocklen;
    do {
      c0 = c0 + *data++;
      c1 = c1 + c0;
    } while ((blocklen -= 2));
    c0 = c0 % 0xFFFFFFFF;
    c1 = c1 % 0xFFFFFFFF;
  }
  return (c1 << 24 | c0);
}
#endif

#ifdef DJB2
// djb2 - Dan bernstein hashing function
uint32_t Checksum::djb2(const uint8_t *data) {
  uint32_t hash = 5381;
  uint8_t d;

  while (d = *data++) {
    hash = ((hash << 5) + hash) + d; 
    // Two alternatives
    //hash = hash * 33 + d;
    //hash = hash * 33 ^ d;
  }

  return hash;
}
#endif

#ifdef BLAKE2
// TODO: Blake2
#endif
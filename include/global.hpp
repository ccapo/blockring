#pragma once

#include <execinfo.h>
#include <csignal>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <openssl/sha.h>
#include <sqlite3.h>

#include "date.hpp"
#include "uuid.hpp"
#include "json.hpp"

// Protocol Version
#define PROTOCOLVERSION (10)

// Security Mode
#ifdef INSECURE
#define SECURITYMODE (0)
#else
// Default: SECURE
#define SECURE
#define SECURITYMODE (1)
#endif

// Algorithm and Difficulty
#ifdef NAIVE8
#define ALGORITHM (0)
#define DIFFICULTYBITS (8)
inline std::uniform_int_distribution<> distrib(0x00, 0xFF);
#elif FLETCHER16
#define ALGORITHM (1)
#define DIFFICULTYBITS (16)
inline std::uniform_int_distribution<> distrib(0x0000, 0xFFFF);
#elif FLETCHER32
#define ALGORITHM (2)
#define DIFFICULTYBITS (32)
inline std::uniform_int_distribution<> distrib(0x00000000, 0xFFFFFFFF);
#elif DJB2
#define ALGORITHM (3)
#define DIFFICULTYBITS (32)
inline std::uniform_int_distribution<> distrib(0x00000000, 0xFFFFFFFF);
#elif FLETCHER64
#define ALGORITHM (4)
#define DIFFICULTYBITS (64)
inline std::uniform_int_distribution<> distrib(0x0000000000000000, 0xFFFFFFFFFFFFFFFF);
#else
// Default: FLETCHER16
#define FLETCHER16
#define ALGORITHM (1)
#define DIFFICULTYBITS (16)
inline std::uniform_int_distribution<> distrib(0x0000, 0xFFFF);
#endif

// for convenience
using json = nlohmann::json;

class Global {
  public:
  Global() {}
  ~Global() {}

  // Debug Mode
#ifdef DEBUG
  static const bool Debug = true;
#else
  static const bool Debug = false;
#endif

  // Return Codes
  static const int Ok = 0;
  static const int Error = 1;
  static const int Warn = 2;

  // Version Control
  static const int Major = 0;
  static const int Minor = 3;
  static const int Revision = 0;

  static void usage() {
    std::cerr << "Usage:\n";
    std::cerr << "  blockring newring                      Create a new ring\n";
    std::cerr << "  blockring getring <ringUUID>           Fetch a ring and blocks\n";
    std::cerr << "  blockring closering <ringUUID>         Close a ring\n";
    std::cerr << "  blockring purgering <ringUUID>         Purge a ring\n";
    std::cerr << "  blockring newblock <ringUUID> <data>   Add a new block to a ring with provided data\n";
    std::cerr << "  blockring purgeblock <blockUUID>       Purge a block\n";
    std::cerr << "  blockring version                      Show version\n";
    std::cerr << "  blockring help                         Show this menu\n";
  }
};
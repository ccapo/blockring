#pragma once

#include "global.hpp"

// Uniform random distribution
inline std::random_device rd;
inline std::mt19937 gen(rd());

/**
 * Tool Class
 */
class Tool {
  public:
  Tool() {}
  ~Tool() {}

  // Random hex string
  static std::string randomHex() {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(DIFFICULTYBITS/4) << distrib(gen);
    return ss.str();
  }

  // Strip hypens from a UUID
  static std::string stripHyphens(const std::string input) {
    std::string output(input);
    output.erase(std::remove(output.begin(), output.end(), '-'), output.end());
    return output;
  }

  // SHA256 hashing algorithm, requires openssl library
  static std::string sha256(const std::string str) {
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(hash[i]);
    }
    return ss.str();
  }

  // Convert created to epoch in milliseconds, then into hexidecimal
  static std::string dateToEpochHex(const std::string d) {
    std::string fmt = "%F %T";
    date::sys_time<std::chrono::milliseconds> tp;
    std::istringstream in(d);
    in >> date::parse(fmt, tp);

    std::stringstream ss;
    ss << std::hex << std::setfill('0') << tp.time_since_epoch().count();

    return ss.str();
  }

  // Convert decimal to hexidecimal
  template< typename T > static std::string decToHex( T i ) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(sizeof(T)*2) << static_cast<uint64_t>(i);
    return ss.str();
  }
};
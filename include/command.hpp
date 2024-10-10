#pragma once

#include "global.hpp"

/**
 * Command Class
 */
class Command {
public:
  // Command Constructor
  Command();

  // Command Destructor
  ~Command();

  // Connect to Database
  void connectDB();

  // Disconnect from Database
  void disconnectDB();

  // New ring
  int newRing();

  // Get ring
  int getRing(std::string ringUUID);
 
  // Close ring
  int closeRing(std::string ringUUID);

  // Purge ring
  int purgeRing(std::string ringUUID);

  // New block
  int newBlock(std::string ringUUID, std::string data);

  // Purge block
  int purgeBlock(std::string blockUUID);

  // Constants
  const uint64_t NonceFeedback = 1000000;

private:
  Database db;
};
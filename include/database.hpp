#pragma once

#include "global.hpp"

// Default Database location
#define DBFILE ("db/sqlite3.db")

/**
 * Ring Class
 */
class Ring {

friend std::ostream& operator<<(std::ostream& o, Ring r) {
  o << "{" << std::endl;
  o << "  version: '" << r.version << "'" << std::endl;
  o << "  mode: '" << r.mode << "'" << std::endl;
  o << "  algorithm: '" << r.algorithm << "'" << std::endl;
  o << "  difficulty: '" << r.difficulty << "'" << std::endl;
  o << "  uuid: '" << r.uuid << "'" << std::endl;
  o << "  status: '" << r.status << "'" << std::endl;
  o << "  created: '" << r.created << "'" << std::endl;
  o << "  updated: '" << r.updated << "'" << std::endl;
  o << "}";
  return o;
}

public:
  std::string version;
  std::string mode;
  std::string algorithm;
  std::string difficulty;
  std::string uuid;
  std::string status;
  std::string created;
  std::string updated;

  Ring() {}

  ~Ring() {}

  void clear() {
    version = "";
    mode = "";
    algorithm = "";
    difficulty = "";
    uuid = "";
    status = "";
    created = "";
    updated = "";
  }
};

/**
 * Block Class
 */
class Block {

friend std::ostream& operator<<(std::ostream& o, Block b) {
  o << "{" << std::endl;
  o << "  uuid: '" << b.uuid << "'" << std::endl;
  o << "  ring_uuid: '" << b.ring_uuid << "'" << std::endl;
  o << "  prev_hash: '" << b.prev_hash << "'" << std::endl;
  o << "  hash: '" << b.hash << "'" << std::endl;
  o << "  data: '" << b.data << "'" << std::endl;
  o << "  nonce: '" << b.nonce << "'" << std::endl;
  o << "  created: '" << b.created << "'" << std::endl;
  o << "}";
  return o;
}

public:
  std::string uuid;
  std::string ring_uuid;
  std::string prev_hash;
  std::string hash;
  std::string data;
  std::string nonce;
  std::string created;

  Block() {}

  ~Block() {}

  void clear() {
    uuid = "";
    ring_uuid = "";
    prev_hash = "";
    hash = "";
    data = "";
    nonce = "";
    created = "";
  }
};

/**
 * Database Class
 */
class Database {
public:
  // Database Constructor
  Database();

  // Database Destructor
  ~Database();

  // Connect to Database
  void connect();

  // Disconnect from Database
  void disconnect();

  // CRUD commands for ring table

  // Create a new ring entry
  int createRing();
  
  // Read ring entry
  int readRing(std::string ringUUID);
  
  // Update ring entry
  int updateRing(std::string ringUUID, std::string status);
  
  // Delete ring entry
  int deleteRing(std::string ringUUID);
  
  // Callback function for sqlite3_exec
  static int readRingCB(void *data, int ncols, char *values[], char *colName[]);

  // CRUD commands for block table

  // Create a new block entry
  int createBlock(std::string ringUUID, std::string data);
  
  // Read block entry
  int readBlock(std::string blockUUID);

  // Read all blocks entries associated with a ring
  int readBlocks(std::string ringUUID, bool includeHash);
  
  // Update block entry
  int updateBlock(std::string blockUUID, std::string prev_hash, std::string hash, std::string nonce);
  
  // Delete block entry
  int deleteBlock(std::string blockUUID);

  // Callback function for sqlite3_exec
  static int readBlockCB(void *data, int ncols, char *values[], char *colName[]);

  // Callback function for sqlite3_exec
  static int readBlocksCB(void *data, int ncols, char *values[], char *colName[]);

  // Static JSON instance for use in callbacks
  static Ring ring;
  static Block block;
  static std::vector<Block> blocks;
  
  // Constants
  const std::string OpenRingStatus = "open";
  const std::string ClosedRingStatus = "closed";
  const std::string PurgedRingStatus = "purged";
  const std::string DefaultBlockNonce = "0000000000000000";

private:
  sqlite3 *db;
  std::string dbName;
};

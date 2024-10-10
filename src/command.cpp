#include "main.hpp"

// Command Constructor
Command::Command() {}

// Command Destructor
Command::~Command() {}

// Connect to Database
void Command::connectDB() {
  db.connect();
}

// Disconnect from Database
void Command::disconnectDB() {
  db.disconnect();
}

// New ring
int Command::newRing() {
  int rc = db.createRing();
  if (rc != Global::Ok) {
    return rc;
  }

  return Global::Ok;
}

// Get ring and associated blocks
int Command::getRing(std::string ringUUID) {
  // Get ring
  int rc = db.readRing(ringUUID);
  if (rc != Global::Ok) {
    return rc;
  }

  // Store ring data in JSON object
  json ringJSON = json::object();
  ringJSON["version"] = Database::ring.version;
  ringJSON["mode"] = Database::ring.mode;
  ringJSON["algorithm"] = Database::ring.algorithm;
  ringJSON["difficulty"] = Database::ring.difficulty;
  ringJSON["uuid"] = Database::ring.uuid;
  ringJSON["status"] = Database::ring.status;
  ringJSON["created"] = Database::ring.created;
  ringJSON["updated"] = Database::ring.updated;
  ringJSON["blocks"] = json::array();

  // Get blocks
  rc = db.readBlocks(ringUUID, true);
  if (rc != Global::Ok) {
    return rc;
  }

  // Store block data in JSON array
  json blockJSON = json::object();
  for (auto &block : Database::blocks) {
    blockJSON["uuid"] = block.uuid;
    blockJSON["prev_hash"] = block.prev_hash;
    blockJSON["hash"] = block.hash;
    blockJSON["data"] = block.data;
    blockJSON["nonce"] = block.nonce;
    blockJSON["created"] = block.created;
    ringJSON["blocks"].push_back(blockJSON);
  }
  
  std::cout << ringJSON.dump() << std::endl;

  return Global::Ok;
}

// Close ring
int Command::closeRing(std::string ringUUID) {
  // Get ring
  int rc = db.readRing(ringUUID);
  if (rc != Global::Ok) {
    return rc;
  }

  // Check if ring is open
  if (Database::ring.status == db.ClosedRingStatus) {
    std::cerr << "Ring is closed" << std::endl;
    return Global::Error;
  }

  // Find all blocks for ring
  rc = db.readBlocks(ringUUID, false);
  if (rc != Global::Ok) {
    return rc;
  }

  // If blocks are found for ring, connect them
  if (Database::blocks.size() > 0) {
    std::string initPrevHash = Database::blocks[0].prev_hash;

#ifdef INSECURE
    // Connect blocks by computing their hash and updating the prev_hash
    // with the hash for the previous block
    std::string prevHash = initPrevHash;
    for (auto &block : Database::blocks) {
      block.prev_hash = prevHash;
      block.hash = Checksum::computeHash(Database::ring, block);
      prevHash = block.hash;
    }

    // Search for hash collision for last and first blocks
    auto &lastBlock = Database::blocks[Database::blocks.size() - 1];
    if (initPrevHash != lastBlock.hash) {
      // TODO: Parallelize with OpenMP
      uint64_t nonce = 0;
      while (initPrevHash != lastBlock.hash) {
        nonce++;
        lastBlock.nonce = Tool::decToHex<uint64_t>(nonce);
        lastBlock.hash = Checksum::computeHash(Database::ring, lastBlock);
        if (nonce % NonceFeedback == 0) std::cout << ".";
      }
    }
#endif

#ifdef SECURE
    // Connect blocks by computing their hash, but search for hash colliion
    // against random prev_hash for subsequent block
    // TODO: Parallelize with OpenMP
    for (int i = 0; i < Database::blocks.size() - 1; i++) {
      auto &block = Database::blocks[i];
      std::string nextPrevHash = Database::blocks[i + 1].prev_hash;
      block.hash = Checksum::computeHash(Database::ring, block);
      if (nextPrevHash != block.hash) {
        uint64_t nonce = 0;
        while (nextPrevHash != block.hash) {
          nonce++;
          block.nonce = Tool::decToHex<uint64_t>(nonce);
          block.hash = Checksum::computeHash(Database::ring, block);
          if (nonce % NonceFeedback == 0) std::cout << ".";
        }
      }
    }

    // Search for hash collision for last and first blocks
    auto &lastBlock = Database::blocks[Database::blocks.size() - 1];
    if (initPrevHash != lastBlock.hash) {
      // TODO: Parallelize with OpenMP
      uint64_t nonce = 0;
      while (initPrevHash != lastBlock.hash) {
        nonce++;
        lastBlock.nonce = Tool::decToHex<uint64_t>(nonce);
        lastBlock.hash = Checksum::computeHash(Database::ring, lastBlock);
        if (nonce % NonceFeedback == 0) std::cout << ".";
      }
    }
#endif

    // Update ring status to closed
    Database::ring.status = "closed";
    rc = db.updateRing(ringUUID, db.ClosedRingStatus);
    if (rc != Global::Ok) {
      return rc;
    }

    // Store ring data in JSON object
    json ringJSON = json::object();
    ringJSON["version"] = Database::ring.version;
    ringJSON["mode"] = Database::ring.mode;
    ringJSON["algorithm"] = Database::ring.algorithm;
    ringJSON["difficulty"] = Database::ring.difficulty;
    ringJSON["uuid"] = Database::ring.uuid;
    ringJSON["status"] = Database::ring.status;
    ringJSON["created"] = Database::ring.created;
    ringJSON["updated"] = Database::ring.updated;
    ringJSON["blocks"] = json::array();

    // Store block data in JSON array
    json blockJSON = json::object();
    for (auto &block : Database::blocks) {
      // Update block data
      rc = db.updateBlock(block.uuid, block.prev_hash, block.hash, block.nonce);
      if (rc != Global::Ok) {
        return rc;
      }

      blockJSON["uuid"] = block.uuid;
      blockJSON["prev_hash"] = block.prev_hash;
      blockJSON["hash"] = block.hash;
      blockJSON["data"] = block.data;
      blockJSON["nonce"] = block.nonce;
      blockJSON["created"] = block.created;
      ringJSON["blocks"].push_back(blockJSON);
    }

    // Output JSON object
    std::cout << std::endl;
    std::cout << ringJSON.dump() << std::endl;
  } else {
    std::cerr << "No blocks associated with ring" << std::endl;
    return Global::Error;
  }

  return Global::Ok;
}

// Purge ring
int Command::purgeRing(std::string ringUUID) {
  // Get ring
  int rc = db.readRing(ringUUID);
  if (rc != Global::Ok) {
    return rc;
  }

  // Abort if ring is closed
  if (Database::ring.status == db.ClosedRingStatus) {
    std::cerr << "Unable to purge a closed ring" << std::endl;
    return Global::Error;
  }

  // Find all blocks for ring
  rc = db.readBlocks(ringUUID, true);
  if (rc != Global::Ok) {
    return rc;
  }

  // If any blocks are found for this ring, purge them
  if (Database::blocks.size() > 0) {
    for (auto &block : Database::blocks) {
      rc = purgeBlock(block.uuid);
      if (rc != Global::Ok) {
        return rc;
      }
    }
  }

  // Purge empty ring
  rc = db.deleteRing(ringUUID);
  if (rc != Global::Ok) {
    return rc;
  }

  std::cout << "PURGE RING OK" << std::endl;
  
  return Global::Ok;
}

// New block associated with ring
int Command::newBlock(std::string ringUUID, std::string data) {
  // Get ring
  int rc = db.readRing(ringUUID);
  if (rc != Global::Ok) {
    return rc;
  }

  // Abort if ring is closed
  if (Database::ring.status == db.ClosedRingStatus) {
    std::cerr << "Unable to add to a closed ring" << std::endl;
    return Global::Error;
  }

  // Create new block
  rc = db.createBlock(ringUUID, data);
  if (rc != Global::Ok) {
    return rc;
  }
  
  return Global::Ok;
}

// Purge block
int Command::purgeBlock(std::string blockUUID) {
  // Get block
  int rc = db.readBlock(blockUUID);
  if (rc != Global::Ok) {
    return rc;
  }

  // Get ring associated with block
  rc = db.readRing(Database::block.ring_uuid);
  if (rc != Global::Ok) {
    return rc;
  }

  // Check if ring is open
  if (Database::ring.status == db.ClosedRingStatus) {
    std::cerr << "Unable to purge block from closed ring" << std::endl;
    return Global::Error;
  }

  // Delete block
  rc = db.deleteBlock(blockUUID);
  if (rc != Global::Ok) {
    return rc;
  }
  
  std::cout << "PURGE BLOCK OK" << std::endl;

  return Global::Ok;
}
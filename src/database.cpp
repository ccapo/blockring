#include "main.hpp"

// Database Constructor
Database::Database() {}

// Database Destructor
Database::~Database() {}

// Connect to Database
void Database::connect() {
  // Store DB filename
  dbName = std::string(DBFILE);

  // Connect to database
  int rc = sqlite3_open(dbName.c_str(), &db);
  if(rc != SQLITE_OK) {
    std::cerr << "Can not open database: " << sqlite3_errmsg(db) << std::endl;
    exit(Global::Error);
  }
}

// Disconnect from Database
void Database::disconnect() {
  // Disconnect from database
  int rc = sqlite3_close(db);
  if(rc != SQLITE_OK) {
    std::cerr << "Can not close database: " << sqlite3_errmsg(db) << std::endl;
    exit(Global::Error);
  }
}

// CRUD commands for ring table

// Create a new ring entry
int Database::createRing() {
  char *errMsg = 0;

  sole::uuid u4 = sole::uuid4();
  std::string ringUUID = u4.str();
  std::string version = Tool::decToHex<uint8_t>(PROTOCOLVERSION);
  std::string mode = Tool::decToHex<uint8_t>(SECURITYMODE);
  std::string algo = Tool::decToHex<uint8_t>(ALGORITHM);
  std::string diffbits = Tool::decToHex<uint16_t>(DIFFICULTYBITS);

  // Create SQL statement
  std::stringstream ss;
  ss << "INSERT INTO ring (version, mode, algorithm, difficulty, uuid, status) VALUES ('" << version << "', '" << mode << "', '" << algo << "', '" << diffbits << "', '" << ringUUID << "', '" << OpenRingStatus << "');";
  std::string sql = ss.str();
  if (Global::Debug) std::cout << sql << std::endl;

  // Start Transaction
  int rc = sqlite3_exec(db, "BEGIN", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // Execute SQL statement
  rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // End Transaction
  rc = sqlite3_exec(db, "COMMIT", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  std::cout << ringUUID << std::endl;
  
  return Global::Ok;
}

// Read a ring entry
int Database::readRing(std::string ringUUID) {
  char *errMsg = 0;

  // Create SQL statement
  std::stringstream ss;
  ss << "SELECT version,mode,algorithm,difficulty,uuid,status,created,updated FROM ring WHERE uuid = '" << ringUUID << "';";
  std::string sql = ss.str();
  if (Global::Debug) std::cout << sql << std::endl;

  // Start Transaction
  int rc = sqlite3_exec(db, "BEGIN", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // Execute SQL statement
  ring.clear();
  rc = sqlite3_exec(db, sql.c_str(), readRingCB, 0, &errMsg);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // End Transaction
  rc = sqlite3_exec(db, "COMMIT", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // Check if ring exists
  if (ring.uuid == "") {
    std::cerr << "Ring does not exist" << std::endl;
    return Global::Error;
  }
  
  return Global::Ok;
}

// Callback function passed to sqlite3_exec
int Database::readRingCB(void *data, int ncols, char *value[], char *colName[]) {
  for(int i = 0; i < ncols; i++) {
    if (0 == strcmp(colName[i], "version")) {
      ring.version = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "mode")) {
      ring.mode = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "algorithm")) {
      ring.algorithm = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "difficulty")) {
      ring.difficulty = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "uuid")) {
      ring.uuid = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "status")) {
      ring.status = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "created")) {
      ring.created = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "updated")) {
      ring.updated = value[i] ? value[i] : "NULL";
    } else {
      std::cerr << "Unrecognized column: " << colName[i] << std::endl;
    }
  }

  return Global::Ok;
}

// Update ring entry
int Database::updateRing(std::string ringUUID, std::string status) {
  char *errMsg = 0;
 
  // Create SQL statement
  std::stringstream ss;
  ss << "UPDATE ring SET status = '" << status << "' WHERE uuid = '" << ringUUID << "';";
  std::string sql = ss.str();
  if (Global::Debug) std::cout << sql << std::endl;

  // Start Transaction
  int rc = sqlite3_exec(db, "BEGIN", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // Execute SQL statement
  rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // End Transaction
  rc = sqlite3_exec(db, "COMMIT", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }
  
  return Global::Ok;
}

// Delete ring entry
int Database::deleteRing(std::string ringUUID) {
  char *errMsg = 0;
 
  // Create SQL statement
  std::stringstream ss;
  ss << "DELETE FROM ring WHERE uuid = '" << ringUUID << "';";
  std::string sql = ss.str();
  if (Global::Debug) std::cout << sql << std::endl;

  // Start Transaction
  int rc = sqlite3_exec(db, "BEGIN", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // Execute SQL statement
  rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // End Transaction
  rc = sqlite3_exec(db, "COMMIT", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }
  
  return Global::Ok;
}

// CRUD commands for block table

// Create a new block entry
int Database::createBlock(std::string ringUUID, std::string data) {
  char *errMsg = 0;

  sole::uuid u4 = sole::uuid4();
  std::string blockUUID = u4.str();
  std::string prevHash = Tool::randomHex();
 
  // Create SQL statement
  std::stringstream ss;
  ss << "INSERT INTO block (uuid, ring_uuid, data, prev_hash, nonce) VALUES ('" << blockUUID << "', '" << ringUUID << "', '" << data << "', '" << prevHash << "', '" << DefaultBlockNonce << "');";
  std::string sql = ss.str();
  if (Global::Debug) std::cout << sql << std::endl;

  // Start Transaction
  int rc = sqlite3_exec(db, "BEGIN", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // Execute SQL statement
  rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // End Transaction
  rc = sqlite3_exec(db, "COMMIT", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  std::cout << blockUUID << std::endl;
  
  return Global::Ok;
}

// Read block entry
int Database::readBlock(std::string blockUUID) {
  char *errMsg = 0;

  // Create SQL statement
  std::stringstream ss;
  ss << "SELECT uuid,ring_uuid,prev_hash,hash,data,nonce,created FROM block WHERE uuid = '" << blockUUID << "';";
  std::string sql = ss.str();
  if (Global::Debug) std::cout << sql << std::endl;

  // Start Transaction
  int rc = sqlite3_exec(db, "BEGIN", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // Execute SQL statement
  block.clear();
  rc = sqlite3_exec(db, sql.c_str(), readBlockCB, 0, &errMsg);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // End Transaction
  rc = sqlite3_exec(db, "COMMIT", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }
  
  // Check if block exists
  if (Database::block.uuid == "") {
    std::cerr << "Block does not exist" << std::endl;
    return Global::Error;
  }

  // Check if block is assoicated with a ring
  if (Database::block.ring_uuid == "") {
    std::cerr << "Block not associated with any ring" << std::endl;
    return Global::Error;
  }

  return Global::Ok;
}

// Callback function passed to sqlite3_exec
int Database::readBlockCB(void *data, int ncols, char *value[], char *colName[]) {
  for(int i = 0; i < ncols; i++) {
    if (0 == strcmp(colName[i], "uuid")) {
      block.uuid = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "ring_uuid")) {
      block.ring_uuid = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "prev_hash")) {
      block.prev_hash = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "hash")) {
      block.hash = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "data")) {
      block.data = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "nonce")) {
      block.nonce = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "created")) {
      block.created = value[i] ? value[i] : "NULL";
    } else {
      std::cerr << "Unrecognized column: " << colName[i] << std::endl;
    }
  }

  return Global::Ok;
}

// Read all blocks associated with a ring
int Database::readBlocks(std::string ringUUID, bool includeHash) {
  char *errMsg = 0;

  // Create SQL statement
  std::stringstream ss;
  if (includeHash) {
    ss << "SELECT uuid,ring_uuid,prev_hash,hash,data,nonce,created FROM block WHERE ring_uuid = '" << ringUUID << "' ORDER BY created ASC;";
  } else {
    ss << "SELECT uuid,ring_uuid,prev_hash,data,nonce,created FROM block WHERE ring_uuid = '" << ringUUID << "' ORDER BY created ASC;";
  }
  std::string sql = ss.str();
  if (Global::Debug) std::cout << sql << std::endl;

  // Start Transaction
  int rc = sqlite3_exec(db, "BEGIN", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // Execute SQL statement
  blocks.clear();
  rc = sqlite3_exec(db, sql.c_str(), readBlocksCB, 0, &errMsg);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // End Transaction
  rc = sqlite3_exec(db, "COMMIT", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }
  
  return Global::Ok;
}

// Callback function passed to sqlite3_exec
int Database::readBlocksCB(void *data, int ncols, char *value[], char *colName[]) {
  block.clear();
  for(int i = 0; i < ncols; i++) {
    if (0 == strcmp(colName[i], "uuid")) {
      block.uuid = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "ring_uuid")) {
      block.ring_uuid = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "prev_hash")) {
      block.prev_hash = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "hash")) {
      block.hash = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "data")) {
      block.data = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "nonce")) {
      block.nonce = value[i] ? value[i] : "NULL";
    } else if (0 == strcmp(colName[i], "created")) {
      block.created = value[i] ? value[i] : "NULL";
    } else {
      std::cerr << "Unrecognized column: " << colName[i] << std::endl;
    }
  }

  blocks.push_back(block);

  return Global::Ok;
}

// Update block entry
int Database::updateBlock(std::string blockUUID, std::string prev_hash, std::string hash, std::string nonce) {
  char *errMsg = 0;
 
  // Create SQL statement
  std::stringstream ss;
  ss << "UPDATE block SET prev_hash = '" << prev_hash << "', hash = '" << hash << "', nonce = '" << nonce << "' WHERE uuid = '" << blockUUID << "';";
  std::string sql = ss.str();
  if (Global::Debug) std::cout << sql << std::endl;

  // Start Transaction
  int rc = sqlite3_exec(db, "BEGIN", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // Execute SQL statement
  rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // End Transaction
  rc = sqlite3_exec(db, "COMMIT", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }
  
  return Global::Ok;
}

// Delete block entry
int Database::deleteBlock(std::string blockUUID) {
  char *errMsg = 0;
 
  // Create SQL statement
  std::stringstream ss;
  ss << "DELETE FROM block WHERE uuid = '" << blockUUID << "';";
  std::string sql = ss.str();
  if (Global::Debug) std::cout << sql << std::endl;

  // Start Transaction
  int rc = sqlite3_exec(db, "BEGIN", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // Execute SQL statement
  rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  // End Transaction
  rc = sqlite3_exec(db, "COMMIT", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error [" << rc << "]: " << errMsg << std::endl;
    sqlite3_free(errMsg);
    return Global::Error;
  }

  return Global::Ok;
}

Ring Database::ring;
Block Database::block;
std::vector<Block> Database::blocks;

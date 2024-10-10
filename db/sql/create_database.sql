--
-- File generated with SQLiteStudio v3.1.1 on Fri Nov 6 00:25:05 2020
--
-- Text encoding used: UTF-8
--
PRAGMA foreign_keys = off;
BEGIN TRANSACTION;

-- Table: block
DROP TABLE IF EXISTS block;

CREATE TABLE block (
    id        INTEGER  PRIMARY KEY AUTOINCREMENT,
    uuid      TEXT     UNIQUE
                       NOT NULL,
    ring_uuid TEXT     REFERENCES ring (uuid) 
                       NOT NULL,
    prev_hash TEXT     NOT NULL
                       DEFAULT "",
    hash      TEXT     NOT NULL
                       DEFAULT "",
    data      BLOB     NOT NULL,
    nonce     TEXT     NOT NULL
                       DEFAULT "",
    timestamp DATETIME DEFAULT ( (STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW') ) ) 
);


-- Table: ring
DROP TABLE IF EXISTS ring;

CREATE TABLE ring (
    id         INTEGER  PRIMARY KEY AUTOINCREMENT,
    version    TEXT     NOT NULL
                        DEFAULT "",
    mode       TEXT     NOT NULL
                        DEFAULT "",
    algorithm  TEXT     NOT NULL
                        DEFAULT "",
    difficulty TEXT     NOT NULL
                        DEFAULT "",
    uuid       TEXT     UNIQUE
                        NOT NULL,
    status     TEXT     NOT NULL
                        DEFAULT open,
    created    DATETIME DEFAULT ( (STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW') ) ),
    updated    DATETIME DEFAULT ( (STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW') ) ) 
);


-- Trigger: updatedRing
DROP TRIGGER IF EXISTS updatedRing;
CREATE TRIGGER updatedRing
         AFTER UPDATE
            ON ring
      FOR EACH ROW
          WHEN NEW.updated = OLD.updated
BEGIN
    UPDATE ring
       SET updated = strftime('%Y-%m-%d %H:%M:%f', 'NOW') 
     WHERE id = OLD.rowid;
END;


COMMIT TRANSACTION;
PRAGMA foreign_keys = on;

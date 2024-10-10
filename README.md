## BlockRing ##

BlockRing is a proof of concept for a self-contained, blockring ring

#### Dependencies ####
* g++ v7.5.0
* OpenSSL v1.1.1g: `sudo apt install libssl-dev`
* SQLite3 v3.31.1: `sudo apt install libsqlite3-dev`
* upx v3.94: `sudo apt install upx-ucl` (Optional)

#### Database ####
* Empty database provided
* SQL script to create a fresh database

#### Configuration ####
* Managed in the `Makefile`
* Security Mode: `SECURE` or `INSECURE`
* Algorithm: `NAIVE8`, `FLETCHER16`, `FLETCHER32`, `FLETCHER64`, `DJB2`

#### Build ####
* `make all`

### Commands ###
* `blockring newring`                      Create a new ring
* `blockring getring <ringUUID>`           Fetch a ring and blocks
* `blockring closering <ringUUID>`         Close a ring
* `blockring purgering <ringUUID>`         Purge a ring
* `blockring newblock <ringUUID> <data>`   Add a new block to a ring
* `blockring purgeblock <blockUUID>`       Purge a block
* `blockring version`                      Show version
* `blockring help`                         Show help menu

### Example ###
1. `blockring newring`

```
0ca60caf-9f16-4475-9c3c-6681b7e48617
```

2. `blockring newblock 0ca60caf-9f16-4475-9c3c-6681b7e48617 "Lorem ipsum dolor sit amet, consectetur adipiscing elit."`
```
cb3637d3-aa57-44d4-b22b-a0d0bd1af708
```

3. `blockring newblock 0ca60caf-9f16-4475-9c3c-6681b7e48617 "Interdum et malesuada fames ac ante ipsum primis in faucibus."`
```
f52afa94-cee2-4b45-b622-c3eb8c02dbbb
```

4. `blockring newblock 0ca60caf-9f16-4475-9c3c-6681b7e48617 "Nunc pharetra elit magna, eu accumsan lorem convallis et."`
```
8998d34d-cdbd-4bbb-a942-00557e56f3e3
```

5. `blockring closering 0ca60caf-9f16-4475-9c3c-6681b7e48617`

```json
{
  "algorithm": "00",
  "blocks": [
    {
      "created": "2020-11-15 03:28:46.712",
      "data": "Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
      "hash": "7a",
      "nonce": "0000000000000000",
      "prev_hash": "a1",
      "uuid": "cb3637d3-aa57-44d4-b22b-a0d0bd1af708"
    },
    {
      "created": "2020-11-15 03:29:10.538",
      "data": "Interdum et malesuada fames ac ante ipsum primis in faucibus.",
      "hash": "36",
      "nonce": "0000000000000000",
      "prev_hash": "7a",
      "uuid": "f52afa94-cee2-4b45-b622-c3eb8c02dbbb"
    },
    {
      "created": "2020-11-15 03:29:23.288",
      "data": "Nunc pharetra elit magna, eu accumsan lorem convallis et.",
      "hash": "a1",
      "nonce": "00000000000299ff",
      "prev_hash": "36",
      "uuid": "8998d34d-cdbd-4bbb-a942-00557e56f3e3"
    }
  ],
  "created": "2020-11-15 03:28:14.488",
  "difficulty": "0008",
  "mode": "00",
  "status": "closed",
  "updated": "2020-11-15 03:28:14.488",
  "uuid": "0ca60caf-9f16-4475-9c3c-6681b7e48617",
  "version": "0a"
}
```

# Proof of Existence #

This is the proof of existence for the BlockRing code as of 2020-11-16, the record can be found here:

* `https://proofofexistence.com/detail/c83d37b16a4a1cac07bc4dfba12620d11e8df6e6bddf9a2004176dbb0aa8a15a`

As well as the transaction on the bitcoin blockchain:

* `https://www.blockchain.com/btc/tx/cc07c498ccf0732b49ad76dc2521a1e7ab39e7f5108ba23ee006bbaa78fee8cb`

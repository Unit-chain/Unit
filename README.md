# Unit
### Blockhain for holding info about transactions inside system Reborn-cash system.
#### Dependencies list:
1. https://docs.rs/ed25519/latest/ed25519/
2. https://docs.rs/rocksdb/latest/rocksdb/
3. https://docs.rs/leveldb/latest/leveldb/
4. https://github.com/koushiro/bip0039
5. https://crates.io/crates/ed25519

#### Important articles:
1. https://en.wikipedia.org/wiki/Berkeley_Packet_Filter
2. https://www.aumasson.jp
3. https://docs1.w3cub.com/rust/book/error-handling/
4. https://learnmeabitcoin.com/technical/blkdat
5. https://live.blockcypher.com/btc/decodetx/

# Steps
## Unit Virtual Machine
## Machine Type: `stack`
> ByteCode example
```
// Bytecode stream: 03 3b 84 00 01 1a 05 68 3b a7 ff f9
// Disassembly:
iconst_0      // 03
istore_0      // 3b
iinc 0, 1     // 84 00 01
iload_0       // 1a
iconst_2      // 05
imul          // 68
istore_0      // 3b
goto -7       // a7 ff f9
```

# Requirements

> Memory: 2GB (due RocksDB)
> OS: MacOS, Linux, Window

# Installing(Unit-chain)

1. Firstly, install `rocksdb` library (download rocksdb last version, into rocksdb folder: `make shared lib` and then `make install-shared INSTALL_PATH=/usr/local/lib`)
2. Secondly, install `nlohmann_json` library
3. Thirdly, build with cmake program in `rocksdb_uvm_support`:
  1. `cd rocksdb_uvm_support`
  2. `mkdir build`
  3. `cd build`
  4. `cmake ..`
  5. `make`
4. After building, need to run it:  `./rocksdb_uvm_support`. It will create column families which UVM needed
5. After column families were created - need to build `UVM`. 

# ToDo:
- [ ] BIP32 & BIP44
- [ ] Pseudonymous
- [x] VM
- [ ] ZK-STARK || ZK-SNARK
- [ ] DECENTRALIZATION
- [ ] GPU counting Proof of Knowledge

# Tests(Unit-chain)

```markdown
Recent tests show that current blockhain is able to operate on ten thousand transaction easily.
UVM proccess with 10.000 transactions consumed:
- 22 Megabytes memory usage
The block with 10,000 transactions was counted in 40 minutes with one thread on the Apple M1.
```

# Dependencies(VM && Unit-chain)

- rocksdb
- boost
- nlohmann json

# VM Opcodes

|hex num|name   		|args |note   													|examples |Stack [before]→[after] |exception |Implemented(true/false) |
|	:---|	:---:		|	----|				:---:										|	----|	----|	----|	----|
|000	|stop   		| - |stop execution of the contract 							| stop |[no change] | - |true |
| 001     | go_to            | int             | goes to another instruction at *branchoffset*                | go_to 2          |[no change] | - |true |
|002	|swap_references   		| int,int         |swaps two references. indexing starts from top of stack   									| swap 1 3 |value1,value2→value2,value1 | - |true |
|003	|iadd 			| num, num |adding value 											| |value1,value2→result | - |true |
|004	|isub 			| num, num |subtract value 											| |value1,value2→result | - |true |
|005	|idivide 		| num, num |devide value 												| |value1,value2→result | - |true |
|006	|imul 			| num, num |multiply value 											| |value1,value2→result | - |true |
|007	|imod 			| int, int |a % b														| |value1,value2→result |- |true |
|008	|ixor 			| int, int |a ^ b														| |value1,value2→result |- |true |
|009	|iinv 			| num |!a															| |value→!value | - |true |
|010	|ilshift		| int, int |a << val													| |value1,value2→result |second parameter  < int32 max value |true |
|011	|irshift		| int |a >> val													| |value1,value2→result |- |true |
|012	|pop 			| Object |pop value													| |value→ | - | true |
|013	|- 			| - |-													| - |- | - | false |
|014	|dup 			| Object |duplicate from top of the stack											| |value→value1,value1 | - | true |
|015	|ior 			| num |a \|\| b | |value1,value2→result | - | true |
|016	|iand 			| int |a & b			 											| |value1,value2→result |- | true |
|017	|-		| - |-													| - |- | - | false |
|018	|osha3 		| Object |sha3_256 value on top of the stack.		 											| |value→sha3(value) | - | false |
|019	|balance 		| string |get balance of address										| |→value | - | false |
|020	|timestamp	| - |get timestamps												| |→value | - | false |
|021	|blockhash	| - |get blockhash												| |→value | - | false |
|022	|chainid 		| - |returns chain_id											| |→value | - | false |
| 023     | create           | -               | create contract                                              |                  | [no change]                          | - | false |
| 024     | destruct         | string          | destruct contract and returns all holdings to their holders  |                  | [no change]                          | - | false |
| 025     | address          | -               | address of current contract                                  |                  |→value | - | false |
| 026     | invalid          | -               | invalid                                                      |                  |                                      | - | false |
| 027     | iconst_0         | int             | push int value 0 onto the stack.                             | i_const          | → 0                                  | - | true |
| 028     | iconst_1         | int             | push int value 1 onto the stack                              |                  | → 1                                  | - | true |
| 029     | iconst_2         | int             | push int value 2 onto the stack                              |                  | → 2                                  | - | true |
| 030     | iconst_3         | int             | push int value 3 onto the stack                              |                  | → 3                                  | - | true |
| 031     | iconst_4         | int             | push int value 4 onto the stack                              |                  | → 4                                  | - | true |
| 032     | u64const_0       | -               | push uint64_t value 0 onto the stack                         |                  | → 0                                  | - | true |
| 033     | u64const_1       | -               | push uint64_t value 1 onto the stack                         |                  | → 1                                  | - | true |
| 034     | u64const_2       | -               | push uint64_t value 2 onto the stack                         |                  | → 2                                  | - | true |
| 035     | u64const_3       | -               | push uint64_t value 3 onto the stack                         |                  | → 3                                  | - | true |
| 036     | u64const_4       | -               | push uint64_t value 4 onto the stack                         |                  | → 4                                  | - | true |
| 037     | u128const_0      | -               | push uint128_t value 0 onto the stack                        |                  | → 0                                  | - | true |
| 038     | u128const_1      | -               | push uint128_t value 1 onto the stack                        |                  | → 1                                  | - | true |
| 039     | u128const_2      | -               | push uint128_t value 2 onto the stack                        |                  | → 2                                  | - | true |
| 040     | u128const_3      | -               | push uint128_t value 3 onto the stack                        |                  | → 3                                  | - | true |
| 041     | u128const_4      | -               | push uint128_t value 4 onto the stack                        |                  | → 4                                  | - | true |
| 042     | u256const_0      | -               | push uint256_t value 0 onto the stack.                       |                  | → 0                                  | - | true |
| 043     | u256const_1      | -               | push uint256_t value 1 onto the stack                        |                  | → 1                                  | - | true |
| 044     | u256const_2      | -               | push uint256_t value 2 onto the stack                        |                  | → 2                                  | - | true |
| 045     | u256const_3      | -               | push uint256_t value 3 onto the stack                        |                  | → 3                                  | - | true |
| 046     | u256const_4      | -               | push uint256_t value 4 onto the stack                        |                  | → 4                                  | - | true |
| 047     | ostore_0 | int             | push object value into local 0                               |                  | [no change]                          | - |true |
| 048     | ostore_1         | int             | push object value into local 1                               |                  | [no change]                          | - |true |
| 049     | ostore_2         | int             | push object value into local 2                               |                  | [no change]                          | - |true |
| 050     | ostore_3         | int             | push object value into local 3                               |                  | [no change]                          | - |true |
| 051     | ldc              | int index       | load #index from blockhain data.                                                                                                                              If program looks like: [60, 1], it'll load first value from blockchain data[^1] onto the stack.                                                                                                                 If program looks like: [60, 2], it'll load second value from blockchain data[^1]onto the stack. |                  | → value                              | - |true |
| 052     | oload_0          | int index       | load value from local_#index onto top of stack               | iload 1          | → value                              | - |true |
| 053     | oload_1          | int index       | load value from local_#index onto top of stack               | iload 1          | → value                              | - |true |
| 054     | oload_2          | int index       | load value from local_#index onto top of stack               | iload 1          | → value                              | - |true |
| 055     | oload_3          | int index       | load value from local_#index onto top of stack               | iload 1          | → value                              | - |true |
| 056     | swap             | -               | swaps two top elements                                       | swap             | value1,value2→value2,value1          | - |true |
| 057     | if_acmpeq        | Object ,Object  | checks if two values (Object or string) on top of the stack are equal. next index - index of next instruction. (a == b) | if_acmpeq 23     | [no change]                          | - | true |
| 058     | if_acmpne        | Object , Object | checks if two values (Object or string) on top of the stack are not equal. next index - index of next instruction. (a != b) | if_acmpne 23     | [no change]                          | - | true |
| 059     | if_icmpeq        | Object, Object  | checks if two integers (int32, uint64, uint128, uint256) on top of the stack are equal. next index - index of next instruction. (a == b) | if_icmpeq 23     | [no change]                          | - | true |
| 060     | if_icmpge        | num,num         | checks if two integers (int32, uint64, uint128, uint256) on top of the stack are greater than or equal. next index - index of next instruction. (a >= b) | if_icmpge 23     | [no change]                          | - | true |
| 061     | if_icmpgt        | num,num         | checks if two integers (int32, uint64, uint128, uint256) on top of the stack are greater than. next index - index of next instruction. (a > b) | if_icmpgt 23     | [no change]                          | - | true |
| 062     | if_icmple        | num,num         | checks if two integers (int32, uint64, uint128, uint256) on top of the stack are less than or equal. next index - index of next instruction. (a <= b) | if_icmple 23     | [no change]                          | - | true |
| 063     | if_icmplt        | num,num         | checks if two integers (int32, uint64, uint128, uint256) on top of the stack are less. next index - index of next instruction. (a < b) | if_icmplt 23     | [no change]                          | - | true |
| 064     | if_icmpne        | num,num         | checks if two integers (int32, uint64, uint128, uint256) on top of the stack are not equal. next index - index of next instruction. (a != b) | if_icmpne 23     | [no change]                          | - | true |
| 065     | ifeq             | num             | checks if integer (int32, uint64, uint128, uint256) on top of the stack are 0. next index - index of next instruction. (a == 0) | ifeq 23          | [no change]                          | - | true |
| 066     | ifge             | num             | checks if integer (int32, uint64, uint128, uint256) on top of the stack greater than or equal to 0. next index - index of next instruction. (a >= 0) | ifge 23          | [no change]                          | - | true |
| 067     | ifgt             | num             | checks if integer (int32, uint64, uint128, uint256) on top of the stack greater than 0. next index - index of next instruction. (a > 0) | ifgt 23          | [no change]                          | - | true |
| 068     | ifle             | num             | checks if integer (int32, uint64, uint128, uint256) on top of the stack less than or equal to 0. next index - index of next instruction. (a <= 0) | ifle 23          | [no change]                          | - | true |
| 069     | iflt             | num             | checks if integer (int32, uint64, uint128, uint256) on top of the stack less than 0. next index - index of next instruction. (a < 0) | iflt 23          | [no change]                          | - | true |
| 070     | ifne             | num             | checks if integer (int32, uint64, uint128, uint256) on top of the stack not equals 0. next index - index of next instruction. (a != 0) | ifne 23          | [no change]                          | - | true |
| 071     | ifnonnull        | Object          | checks if value (Object or string) on top of the stack are not null. next index - index of next instruction. (a != null) | ifnonnull 23     | [no change]                          | - | true |
| 072     | ifnull           | Object          | checks if value (Object or string) on top of the stack are not null. next index - index of next instruction. (a == null) | ifnull 23        | [no change]                          | - | true |
| 073     | nop              | -               | perform no operation                                         | nop              | [no change]                          | - | true |
| 074     | collectionlength | collection      | push array length on top of the stack by getting length of collection which is on top of the stack | collectionlength | collection→collection,length(uint64) | - | true |
| 075     | dconst_0         | -               | push int value 0.0 onto the stack.                           | dconst_0         | → 0.0                                | - | true |
| 076     | dconst_1         | -               | push int value 1.0 onto the stack.                           | dconst_1         | → 1.0                                | - | true |
| 077     | u64add           | num, num        | adding value                                                 |                  | value1,value2→value1+value2          | - |true |
| 078     | u128add          | num, num        | adding value                                                 |                  | value1,value2→value1+value2          | - |true |
| 079     | u256add          | num, num        | adding value                                                 |                  | value1,value2→value1+value2          | - |true |
| 080     | dadd             | num, num        | adding value                                                 |                  | value1,value2→value1+value2          | - |true |
| 081     | u64sub           | num, num        | substract value                                              |                  | value1,value2→value1-value2          | - |true |
| 082     | u128sub          | num, num        | substract value                                              |                  | value1,value2→value1-value2          | - |true |
| 083     | u256sub          | num, num        | substract value                                              |                  | value1,value2→value1-value2          | - |true |
| 084     | dsub             | num, num        | substract value                                              |                  |value1,value2→value1-value2 | - |true |
| 085 | u64divide | num, num | devide value | |value1,value2→value1/value2 | - |true |
| 086 | u64mul | num, num | multiply value | |value1,value2→value1*value2 | - |true |
| 087 | u64mod | int, int | a % b | |value1,value2→value1%value2 |double |true |
| 088 | u64xor | int, int | a ^ b | |value1,value2→value1^value2 |double |true |
| 089 | u64inv | num | !a | |value→!value | - |true |
| 090 | u64lshift | int, int | a << val | |value1,value2→value1<<value2 |second parameter  < int32 max value |true |
| 091 | u128divide | num, num | devide value | |value1,value2→value1/value2 | - |true |
| 092 | u128mul | num, num | multiply value | |value1,value2→value1*value2 | - |true |
| 093 | u128mod | int, int | a % b | |value1,value2→value1%value2 |double |true |
| 094 | u128xor | int, int | a ^ b | |value1,value2→value1^value2 |double |true |
| 095 | u128inv | num | !a | |value→!value | - |true |
| 096     | u256divide       | num, num        | devide value                                                 |                  | value1,value2→value1/value2          | -                                   |true |
| 097     | u256mul          | num, num        | multiply value                                               |                  | value1,value2→value1*value2          | -                                   | true                    |
| 098     | u256mod          | int, int        | a % b                                                        |                  | value1,value2→value1%value2          | double                              | true                    |
| 099     | u256xor          | int, int        | a ^ b                                                        |                  | value1,value2→value1^value2          | double                              | true                    |
| 100     | u256inv          | num             | !a                                                           |                  | value→!value                         | -                                   | true                    |
| 101     | u256lshift       | int, int        | a << val                                                     |                  | value1,value2→value1<<value2         | second parameter  < int32 max value | true                    |
| 102     | dinv             | num             | !a                                                           |                  | value→!value                         | -                                   | true                    |
| 103     | ddivide          | num, num        | devide value                                                 |                  | value1,value2→value1/value2          | -                                   | true                    |
| 104     | dmul             | num, num        | multiply value                                               |                  | value1,value2→value1*value2          | -                                   | true                   |
| 105     | u64or            | num             | a \|\| b                                                     |                  | value1,value2→value1\|\|value2       | -                                   | true |
| 106     | u64and           | int             | a & b                                                        |                  | value1,value2→value1&value2          | double                              | true |
| 107     | u128or           | num             | a \|\| b                                                     |                  | value1,value2→value1\|\|value2       | -                                   | true |
| 108     | u128and          | int             | a & b                                                        |                  | value1,value2→value1&value2          | double                              | true |
| 109     | u256or           | num             | a \|\| b                                                     |                  | value1,value2→value1\|\|value2       | -                                   | true |
| 110     | u256and          | int             | a & b                                                        |                  | value1,value2→value1&value2          | double                              | true |
| 111     | dor              | num             | a \|\| b                                                     |                  | value1,value2→value1\|\|value2       | -                                   | true |
| 112     | iinc             | int, num        | a + increasing_value(e.g 1,2,3,4,5,6,7,8,9 etc). First parameter - is index of local(e.g 1,2,3,4), second - increasing value. In example we're increasing value in local#1 by 4, so if we have in local#1, for example value 6, it'll become 10 after instruction | iinc 1 4         | [No change]                          | -                                   | true |
| 113     | i2d              | num,num         | convert int to double (two top values from stack)            | i2d              | value→result                         | -                                   | true |
| 114     | i2u64            | num,num         | convert int to uint64 (two top values from stack)            | i2u64            | value→result                         | -                                   | true |
| 115     | i2u128           | num,num         | convert int to uint128 (two top values from stack)           | i2u128           | value→result                         | -                                   | true |
| 116     | i2u256           | num,num         | convert int to uint256 (two top values from stack)           | i2u256           | value→result                         | -                                   | true |
| 117     | d2i              | num,num         | convert double to int (two top values from stack)            | d2i              | value→result                         | -                                   | true |
| 118     | d2u64            | num,num         | convert double to uint64 (two top values from stack)         | d2u64            | value→result                         | -                                   | true |
| 119     | d2u128           | num,num         | convert double to uint128 (two top values from stack)        | d2u128           | value→result                         | -                                   | true |
| 120     | d2u256           | num,num         | convert double to uint256 (two top values from stack)        | d2u256           | value→result                         | -                                   | true |
| 121     | u64_2u128        | num,num         | convert uint64 to uint128 (two top values from stack)        | u64_2u128        | value→result                         | -                                   | true |
| 122     | u64_2u256        | num,num         | convert uint64 to uint256 (two top values from stack)        | u64_2u256        | value→result                         | -                                   | true |
| 123     | u128_2u256       | num,num         | convert uint128 to uint256 (two top values from stack)       | u128_2u256       | value→result                         | -                                   | true |
| 124     | u128lshift       | int, int        | a << val                                                     |                  | value1,value2→value1<<value2         | second parameter  < int32 max value |true |
| 125 | u64rshift | int, int | a << val | | value1,value2→value1<<value2 | second parameter  < int32 max value |true |
| 125 | u128rshift | int, int | a << val | | value1,value2→value1<<value2 | second parameter  < int32 max value |true |
| 126 | u256rshift | int, int | a << val | | value1,value2→value1<<value2 | second parameter  < int32 max value |true |
| 127 | iconst_5 | - | - | iconst_5 | → 5 | - | true |
| 128 | u64const_5 | - | - | u64const_5 | → 5 | - | true |
| 129 | u128const_5 | - | - | u128const_5 | → 5 | - | true |
| 130 | u256const_5 | - | - | u256const_5 | → 5 | - | true |
| 131 | u64inc | int,int | a + increasing_value(e.g 1,2,3,4,5,6,7,8,9 etc). First parameter - is index of local(e.g 1,2,3,4), second - increasing value. In example we're increasing value in local#1 by 4, so if we have in local#1, for example value 6, it'll become 10 after instruction | u64inc 1 4 | [No change] | - | true |
| 131 | u128inc | int,int | a + increasing_value(e.g 1,2,3,4,5,6,7,8,9 etc). First parameter - is index of local(e.g 1,2,3,4), second - increasing value. In example we're increasing value in local#1 by 4, so if we have in local#1, for example value 6, it'll become 10 after instruction | u128inc 1 4 | [No change] | - | true |
| 132 |                  | int,int         | a + increasing_value(e.g 1,2,3,4,5,6,7,8,9 etc). First parameter - is index of local(e.g 1,2,3,4), second - increasing value. In example we're increasing value in local#1 by 4, so if we have in local#1, for example value 6, it'll become 10 after instruction | u256inc 1 4      | [No change]                          | -                                   | true |

[^1]: values array




# Vm Data Structures

| num  | name       | sizeof(class name)                 | note                                                         |
| ---- | ---------- | ---------------------------------- | ------------------------------------------------------------ |
| 1    | i32        | 40 bytes                           |                                                              |
| 2    | u64        | 8 bytes                            |                                                              |
| 3    | u128       | 80 bytes                           |                                                              |
| 4    | u256       | 96 bytes                           |                                                              |
| 5    | d64        | 40 bytes                           | number of decimal places is 4(due c++'s converting from string to double) |
| 6    | boolean    | 40 bytes                           |                                                              |
| 7    | collection | sum of sizes all structures inside |                                                              |
| 8    | str        | 1 byte * length (ASCII)            |                                                              |
| 9    | Object     | 40 bytes                           |                                                              |

# VM Technical Structures
|Num   |name   		|note   												     |
|	:---		|	:---:		 |				:---:											|
|1	|UniqueConstantPool |Contains functions and values to be loaded(unique for each program). |
|2	|SharedConstatnPool |Contains precompiled functions, values(e.g SHA3 in the future, uint256 max value etc)(shared between programs) |

# Examples of programs for UVM

## Test №1

> Time: 0 ms

```c++
std::string code = R"({"functions": [{"test": [28, 28, 3, 0]}], "values": [{"value":"test","type":0}]})";
UniqueConstantPool uniqueConstantPool = UniqueConstantPool(code);
std::string function = "test";
VM vm = VM(10, code);
boost::json::array test = uniqueConstantPool.loadFunction(function).value();
VM::VMRun(&vm, test);
std::cout << vm.stack.back(); // 2
```

## Test №2

> Time: 0 ms

```c++
std::string code = R"({"functions": [{"test": [29, 29, 128, 29, 6, 6, 6, 0]}], "values": [{"value":"test","type":0}]})";
UniqueConstantPool uniqueConstantPool = UniqueConstantPool(code);
std::string function = "test";
VM vm = VM(10, code);
boost::json::array test = uniqueConstantPool.loadFunction(function).value();
VM::VMRun(&vm, test);
std::cout << vm.stack.back(); // 40
```

## Test №3

> Time: 0 ms

```c++
std::string code = R"({"functions": [{"test": [29, 29, 128, 29, 6, 6, 6, 29, 29, 128, 29, 6, 6, 6, 29, 59, 18, 6, 0]}], "values": [{"value":"test","type":0}]})";
UniqueConstantPool uniqueConstantPool = UniqueConstantPool(code);
std::string function = "test";
VM vm = VM(10, code);
boost::json::array test = uniqueConstantPool.loadFunction(function).value();
VM::VMRun(&vm, test);
std::cout << vm.stack.back() << std::endl; // 2
// values on top of stack are not equal(e.g 40, 2)

// if program looks like this
std::string code = R"({"functions": [{"test": [29, 29, 128, 29, 6, 6, 6, 29, 29, 128, 29, 6, 6, 6, 59, 17, 6, 0]}], "values": [{"value":"test","type":0}]})";
UniqueConstantPool uniqueConstantPool = UniqueConstantPool(code);
std::string function = "test";
VM vm = VM(10, code);
boost::json::array test = uniqueConstantPool.loadFunction(function).value();
VM::VMRun(&vm, test);
std::cout << vm.stack.back() << std::endl; // 1600
// values on top of stack are equal(e.g 40, 40)
```



## Test №4

> Time: 0 ms

```c++
std::string code = R"({"functions": [{"test": [27, 48, 53, 29, 62, 13, 112, 1, 1, 1, 3, 0]}], "values": [{"value":"test","type":0}]})";
UniqueConstantPool uniqueConstantPool = UniqueConstantPool(code);
std::string function = "test";
VM vm = VM(10, code);
boost::json::array test = uniqueConstantPool.loadFunction(function).value();
VM::VMRun(&vm, test);
std::cout << vm.stack.back() << std::endl; // 2
// [27, 48, 53, 29, 62, 13, 112, 1, 1, 1, 3, 0] is equivalent to 
for (int i = 0; i < 2; i++) {}
```

# Tests (without program code examples) UVM


## Test #1

## In this test was used only two operations: 76(dconst_1) and 80(dadd). Both of them was invoked one million times. First million was dconst_1, second million - dadd. 

> CPU: Apple M1(arm64)
> Time: 1556 ms(including all constructors and desctructors)
>
> Memory usage: 43Mb

## Test #2 

## In this test was used only two operations: 28(iconst_1) and 3(iadd). Both of them was invoked one million times. First million was iconst_1, second million - iadd. 

> CPU: Apple M1(arm64)
> Time: 1472 ms(including all constructors and desctructors)
>
> Memory usage: 48Mb

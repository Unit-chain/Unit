# Installing
1. Firstly, install `rocksdb` library
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
- [ ] VM
- [ ] ZK-STARK
- [ ] DECENTRALIZATION
- [ ] GPU counting Proof of Knowledge

# Tests

```markdown
Recent tests show that current blockhain is able to operate on ***ten thousand*** transaction easily.
**UVM** proccess with 10.000 transactions consumed:
- 22 Megabytes memory usage
- 13 threads
Block with 10.000 transactions was counted by 40 minutes with one thread on Apple M1.
```

# Dependencies

- rocksdb
- nlohmann_json
- boost

# VM Opcodes

|hex num|name   		|note   													|
|	:---|	:---:		|				:---:										|
|000	|stop   		|stop execution of the contract 							|
|001	|loop   		|-   														|
|002	|swap   		|swaps two references   									|
|003	|inc 			|increase value 											|
|004	|sub 			|substract value 											|
|005	|divide 		|devide value 												|
|006	|mul 			|multiply value 											|
|007	|i_div 			|a % b														|
|008	|i_xor 			|a ^ b														|
|009	|inv 			|!a															|
|010	|l_shift		|a << val													|
|011	|r_shift		|a >> val													|
|012	|pop 			|pop value													|
|013	|push 			|push value													|
|014	|dup 			|duplicate(memcpy)											|
|015	|i_or 			|a |
|016	|i_and 			|a & b			 											|
|017	|i_return		|return;													|
|018	|i_sha3 		|sha3_256		 											|
|019	|i_balance 		|get balance of address										|
|020	|i_timestamp	|get timestamps												|
|021	|i_blockhash	|get blockhash												|
|022	|i_chainid 		|returns chain_id											|
|023	|i_memory_store	|store value in memory		 								|
|024	|i_memory_value|return;													|
|025	|i_storage_store|stores key : value		 									|
|026	|create 		|create contract 											|
|027	|destruct 		|destruct contract and returns all holdings to their holders|
|028	|jump 			|jumps to instruction <num> in stack						|
|029	|equal |a  == b	|
|030	|gt |a > b	|
|031	|lt |a < b	|
|032	|egt |a >= b	|
|033	|elt |a <=b	|
|034	|address |address of current contract	|
|035	|invalid |invalid	|


​	
# VM Structures
|Num   |name   		|note   												     |
|	:---		|	:---:		 |				:---:											|
|0			   	 |instruction_stack   		|Stack with all instructions of the program 				  |
|1	|program_counter |it contains an address of currently executed instruction. Program will be executed by moving PC through instruction set, reading opcodes and executing them |
|2	|stack_pointer |it contains information about number of elements stored on the stack. It always points on the top of it |
|3	|frame_pointer |frame pointer (for local scope) |


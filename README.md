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
- [ ] Pseudonymous
- [ ] VM
- [ ] GPU counting Proof of Knowledge

# Dependencies
- rocksdb
- nlohmann_json

# VM Opcodes

|hex num|name   		|note   													|
|	---	|	---			|				---											|
|000	|stop   		|stop execution of the contract 							|
|001	|loop   		|-   														|
|002	|swap   		|swaps two references   									|
|003	|inc 			|increase value 											|
|004	|sub 			|substract value 											|
|005	|devide 		|devide value 												|
|006	|mul 			|multiply value 											|
|007	|i_div 			|a % b														|
|008	|i_xor 			|a ^ b														|
|009	|inv 			|!a															|
|010	|l_shift		|a << val													|
|011	|r_shift		|a >> val													|
|012	|pop 			|pop value													|
|013	|push 			|push value													|
|014	|dup 			|duplicate(memcpy)											|
|015	|i_or 			|a || b		 												|
|016	|i_and 			|a & b			 											|
|017	|i_return		|return;													|
|018	|i_sha3 		|sha3_384		 											|
|019	|i_balance 		|get balance of address										|
|020	|i_timestamp	|get timestamps												|
|021	|i_blockhash	|get blockhash												|
|022	|i_chainid 		|returns chain_id											|
|023	|i_memory_store	|store value in memory		 								|
|024	|in memory value|return;													|
|025	|i_storage_store|stores key : value		 									|
|026	|create 		|create contract 											|
|027	|destruct 		|destruct contract and returns all holdings to their holders|
|028	|jump 			|jumps to instruction <num> in stack						|

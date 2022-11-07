# Allowed methods

***null*** - nothing needs to be used as parameters.

***address*** - address of wallet(e.g UNTxp28nf2wLJzWm49YtLKyXGYh8vJ3).

***transaction pool*** - pool of transactions should be inserted into blocks(**note**: they also can be declined due many reasons; **note 2**: txs can be inserted ).

| *Method name*               | *Parameters*                          | *Description*                                                |
| --------------------------- | ------------------------------------- | ------------------------------------------------------------ |
| transfer                    | transaction data(e.g tx view in json) | Transfering units or tokens from address under field "from" to address under field "to". |
| unit_get_balance            | address                               | Returns current amount of units & tokens.                    |
| unit_get_tx_pool_size       | null                                  | Returns integer value which shows how many transactions on current node in pool. |
| unit_get_address_tx_history | address                               | Returns array of hashes of txs under two fields: **out**, **in**, out means outcominng txs, in means incoming txs. |
| unit_get_block_height       | null                                  | Returns integer value of latest block index.                 |



* transfer

```json
{"jsonrpc":"2.0", "method":"transfer", "params": 
 {
  "from": "UNTxp28nf2wLJzWm49YtLKyXGYh8vJ3",
  "to": "UNTxspt199oNWgDTHhx153BoUPMxY6D",
  "amount": 1e3,
  "type": 1,
  "extradata": {
  "name": "null",
  "value": "null",
  "bytecode":"null"
  }
 }
```

```json
{"jsonrpc":"2.0", "method":"transfer", "params": 
 {
  "from": "UNTxp28nf2wLJzWm49YtLKyXGYh8vJ3",
  "to": "UNTxspt199oNWgDTHhx153BoUPMxY6D",
  "amount": 0,
  "type": 1,
  "extradata": {
  "name": "token",
  "value": "1e3",
  "bytecode":"null"
  }
 }
```

* unit_get_balance

```json
{"jsonrpc":"2.0", "method":"unit_get_balance", "params":"UNTxp28nf2wLJzWm49YtLKyXGYh8vJ3"}
```

* unit_get_tx_pool_size

```json
{"jsonrpc":"2.0", "method":"unit_get_tx_pool_size", "params":null}
```

* unit_get_address_tx_history

```json
{"jsonrpc":"2.0", "method":"unit_get_address_tx_history", "params":"UNTxp28nf2wLJzWm49YtLKyXGYh8vJ3"}
```

* unit_get_block_height

```json
{"jsonrpc":"2.0", "method":"unit_get_block_height", "params":null}
```

# Error codes

| ***Code name*** | ***Meaning***     |
| --------------- | ----------------- |
| -32700          | parse error.      |
| -32600          | invalid request.  |
| -32601          | method not found. |
| -32602          | invalid parameter |
| -32000          | low balance       |
| -32001          | token exists      |
| -32002          | balance not found |


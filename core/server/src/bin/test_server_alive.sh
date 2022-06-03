#!/bin/bash
echo pwd
curl --location --request POST 'http://localhost:8080' \
--output "output.txt" \
--header 'Content-Type: text/plain' \
--data-raw '{
            "hash": "some haaash",
            "data": 232,
            "from": "From your mom!",
            "to": "To your mom!",
            "amount": 100000000,
            "tx_type": 2,
            "typevalue": {
              "name": "Name",
              "value": 6
            },
            "sign": "BIG SIIIIIGN",
            "previous": "some sign, meh",
            "blockid": 4,
            "epoch": 5
          }' 
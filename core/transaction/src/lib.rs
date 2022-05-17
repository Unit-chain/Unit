#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}

pub mod transaction {
    pub struct NameValue {
        pub name: String,
        pub value: u64,
    }
    pub struct Transaction {
        pub hash: String,
        pub data: u128,
        pub from: String,
        pub to: String,
        pub amount: u128,
        pub tx_type: u64,
        pub typevalue: NameValue,
        pub sign: String,
        pub previous: String,
    }
    impl Transaction {
        pub fn new(
            hash: String,
            data: u128,
            from: String,
            to: String,
            amount: u128,
            tx_type: u64,
            typevalue: NameValue,
            sign: String,
            previous: String,
        ) -> Transaction {
            Transaction {
                hash,
                data,
                from,
                to,
                amount,
                tx_type,
                typevalue,
                sign,
                previous,
            }
        }
    }
}

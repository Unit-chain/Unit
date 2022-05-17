#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}


pub mod instructions {
    pub enum Instructions {
        DEPLOY_STRUCTURE,
        GET_BALANCE,
        SEND_TRANSACTION,
        CONVERT_INTO_STRUCTURE,
        CONVERT_INTO_UNIT,
        FIND_BLOCK,
        BURN_UNITS,
        BURN_STRUCTURE_TOKENS,
        GET_STRUCTURE_INFO
    }
}
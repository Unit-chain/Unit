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
        deploy_structure,
        get_balance,
        send_transaction,
        convert_into_structure,
        convert_into_unit,
        find_block,
        burn_units,
        burn_structure_tokens,
        get_structure_info
    }
}
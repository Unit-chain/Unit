#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
}

use serde::{Deserialize, Serialize};
pub trait Serializable {
    fn serialize(&self) -> String {
        String::from("(Read more...)")
    }
}

// impl Serializable for T {
//     use serde::{Deserialize, Serialize};
//     fn serialize(&self) -> String {
//         String::from(serde_json::to_string(&self).unwrap())
//     }
// }

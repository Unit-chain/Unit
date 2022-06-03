use server::ThreadPool;
use std::fs;
use std::io::prelude::*;
use std::net::TcpListener;
use std::net::TcpStream;
#[path = "../../../instructions/src/lib.rs"]
pub mod instructions;
#[path = "../../../transaction/src/lib.rs"]
pub mod transaction;

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2 + 2;
        assert_eq!(result, 4);
    }
    // Http request to the WORKING server
    #[ignore]
    #[test]
    fn server_alive() {
        use super::*;
        use std::fs::File;
        use std::path::Path;
        use std::process::Command;
        // Request
        let cmd = Command::new("bash")
            .arg("./src/bin/test_server_alive.sh")
            .output();
        _ = cmd;

        // let mut pwd = Command::new("pwd");
        // let pwd = pwd.output();
        // let pwd = match pwd {
        //     Ok(p) => p,
        //     Err(err) => panic!("{}", err),
        // };
        // println!("pwd = {:?}", pwd);

        // Open file
        let path = Path::new("../server/output.txt");
        let display = path.display();
        let mut file = match File::open(&path) {
            Err(why) => panic!("couldn't open {}: {}", display, why),
            Ok(file) => file,
        };

        // Read the file contents into a string, returns `io::Result<usize>`
        let mut s = String::new();
        match file.read_to_string(&mut s) {
            Err(why) => panic!("couldn't read {}: {}", display, why),
            Ok(_) => print!("{} contains:\n{}", display, s),
        }
        // Delete output.txt file
        let _ = Command::new("rm").arg("../server/output.txt").output();
        assert_eq!(
            r#"{"hash":"some haaash","data":232,"from":"From your mom!","to":"To your mom!","amount":100000000,"tx_type":2,"typevalue":{"name":"Name","value":6},"sign":"BIG SIIIIIGN","previous":"some sign, meh","blockid":4,"epoch":5}"#,
            s
        )
    }
    #[ignore]
    #[test]
    fn server_transaction_deserialize() {
        assert_eq!(1, 2);
    }
    #[ignore]
    #[test]
    fn server_transaction_receive() {
        assert_eq!(1, 2);
    }
    #[test]
    fn match_request() {
        use super::match_request;
        use super::Instructions;

        // Below - unusual test cases
        assert_eq!(
            match_request(String::from("Hello sweety")),
            Instructions::wallet_listen
        );

        // Below tests is use cases
        assert_eq!(
            match_request(String::from("deploy_structure")),
            Instructions::deploy_structure
        );
        assert_eq!(
            match_request(String::from("get_balance")),
            Instructions::get_balance
        );
        assert_eq!(
            match_request(String::from("send_transaction")),
            Instructions::send_transaction
        );
        assert_eq!(
            match_request(String::from("convert_into_structure")),
            Instructions::convert_into_structure
        );
        assert_eq!(
            match_request(String::from("convert_into_unit")),
            Instructions::convert_into_unit
        );
        assert_eq!(
            match_request(String::from("find_block")),
            Instructions::find_block
        );
        assert_eq!(
            match_request(String::from("burn_units")),
            Instructions::burn_units
        );
        assert_eq!(
            match_request(String::from("burn_structure_tokens")),
            Instructions::burn_structure_tokens
        );
        assert_eq!(
            match_request(String::from("get_structure_info")),
            Instructions::get_structure_info
        );
        assert_eq!(
            match_request(String::from("create_wallet")),
            Instructions::create_wallet
        );
        assert_eq!(
            match_request(String::from("wallet_listen")),
            Instructions::wallet_listen
        );
    }
}

fn main() {
    let listener = TcpListener::bind("127.0.0.1:8080").unwrap();
    let pool = ThreadPool::new(4);
    println!("{}", "Server launched");
    for stream in listener.incoming() {
        let stream = stream.unwrap();

        pool.execute(|| {
            handle_connection(stream);
        });
    }

    println!("Shutting down.");
}
pub use instructions::instructions::Instructions;
fn match_request(request: String) -> Instructions {
    if request == String::from("deploy_structure") {
        Instructions::deploy_structure
    } else if request == String::from("get_balance") {
        Instructions::get_balance
    } else if request == String::from("send_transaction") {
        Instructions::send_transaction
    } else if request == String::from("convert_into_unit") {
        Instructions::convert_into_unit
    } else if request == String::from("find_block") {
        Instructions::find_block
    } else if request == String::from("burn_units") {
        Instructions::burn_units
    } else if request == String::from("burn_structure_tokens") {
        Instructions::burn_structure_tokens
    } else if request == String::from("get_structure_info") {
        Instructions::get_structure_info
    } else if request == String::from("create_wallet") {
        Instructions::create_wallet
    } else if request == String::from("convert_into_structure") {
        Instructions::convert_into_structure
    } else {
        Instructions::wallet_listen
    }
}

pub fn html_body_to_json(body: String) -> String {
    let mut fl: bool = false;
    let mut answer: String = "".to_string();

    let mut count = 0;

    for letter in body.chars() {
        if letter == '{' {
            fl = true;
            count += 1;
        }
        if letter == '}' {
            count -= 1;
        }
        if letter == '\n' {
            continue;
        }
        if fl {
            answer += &letter.to_string();
        }
        if count == 0 && fl {
            break;
        }
    }
    String::from(answer)
}

fn handle_connection(mut stream: TcpStream) {
    use transaction::transaction;

    let mut buffer = [0; 1024];
    stream.read(&mut buffer).unwrap();

    let get = b"GET / HTTP/1.1\r\n";
    let post = b"POST / HTTP/1.1\r\n";
    let mut response: String;

    if buffer.starts_with(get) {
        let contents = fs::read_to_string("hello.html").unwrap();
        response = format!(
            "HTTP/1.1 200 OK\r\nContent-Length: {}\r\n\r\n{}",
            contents.len(),
            contents
        );
    } else if buffer.starts_with(post) {
        response = format!("{}", String::from_utf8_lossy(&buffer[..]));
        let answer_json: String = html_body_to_json(response);

        let transaction: transaction::Transaction;

        transaction = transaction::deserialize_transaction(format!("{}", answer_json));
        println!("\nDeserialized transaction {}", transaction.serialize());
        response = format!("HTTP/1.1 200 OK\r\n\r\n{}", transaction.serialize());
    } else {
        let contents = fs::read_to_string("404.html").unwrap();
        response = format!(
            "HTTP/1.1 404 NOT FOUND\r\nContent-Length: {}\r\n\r\n{}",
            contents.len(),
            contents
        );
    };

    let response_as_bytes = response.as_bytes();
    stream.write(response_as_bytes).unwrap();
    stream.flush().unwrap();
}

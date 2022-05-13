# Rust
### Владение переменными
1. Когда используется переменная, чтобы не передавать право владения ею функции - необходимо передать ссылку на эту переменную. 
2. Когда используется переменная, чтобы передать право владения ею функции, не нужно использовать ссылку
```rust
// move_semantics6.rs
// Make me compile! `rustlings hint move_semantics6` for hints
// You can't change anything except adding or removing references

fn main() {
    let data = "Rust is great!".to_string();

    get_char(&data);

    string_uppercase(data);
}

// Should not take ownership
fn get_char(mut data: &String) -> char {
    data.chars().last().unwrap()
}

// Should take ownership
fn string_uppercase(mut data: String) {
    data = data.to_uppercase();

    println!("{}", data);
}

```
3. Потоковые срезы
```rust
let s = String::from("hello world");
let hello = &s[0..5];
let world = &s[6..11];
let slice = &s[..2]; // start from 0
let slice = &s[..]; // start from 0 to end of string
////////////////////////////////////////////////////
// Arrays
let a = [1, 2, 3, 4, 5];
let slice = &a[1..3];
assert_eq!(slice, &[2, 3]); // true
```
> Эта инициализация похожа на создание ссылки на переменную String, но с дополнительным условием - указанием отрезка [0..5]. Вместо ссылки на всю String,     срез ссылается на её часть.

![slice-link](https://raw.githubusercontent.com/Abuzik/reborn-block/0902656ed67519e912e28df46a30ffd928a6d3c1/Rust-helps/images/trpl04-06.svg "Slice")

### Кортежи
1. Если кортеж содержит несколько типов переменных одновременно, можно получить поля этих кортежей одной строкой.
```rust
    let cat = ("Furry McFurson", 3.5);
    let (name, age) = cat;

    println!("{} is {} years old.", name, age);
    // Output: Furry McFurson is 3.5 years old. 
///////////////////////////////////////////////
    let tup: (i32, f64, u8) = (500, 6.4, 1);
    let (x, y, z) = tup;
    println!("The value of y is: {y}");
    // Output: The value of y is: 6.4
```

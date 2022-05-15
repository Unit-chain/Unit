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
///////////////////////////////////////////////
    let x: (i32, f64, u8) = (500, 6.4, 1);
    let five_hundred = x.0; // 500
    let six_point_four = x.1; // 6.4
    let one = x.2; // 1
```
### Структуры
1. Структуры определяются как ```struct```.
```rust
struct User {
    active: bool,
    username: String,
    email: String,
    sign_in_count: u64,
}
```
> Инициализируются через:
```rust
    let user1 = User {
        email: String::from("someone@example.com"),
        username: String::from("someusername123"),
        active: true,
        sign_in_count: 1,
    };
    user1.email = String::from("anotheremail@example.com");
    ///////////////////////////////////////////////////////
    fn build_user(email: String, username: String) -> User {
        User {
            email: email,
            username: username,
            active: true,
            sign_in_count: 1,
        }
    }
    // Short initialization. Without adding field name, like in previous example(email: email)
    fn build_user(email: String, username: String) -> User {
        User {
            email,
            username,
            active: true,
            sign_in_count: 1,
        }
    }
```
2. Сокращенная инициализация:
```rust
   // long initialization
   let user2 = User {
        active: user1.active,
        username: user1.username,
        email: String::from("another@example.com"),
        sign_in_count: user1.sign_in_count,
    };

    // short initialization
    let user2 = User {
        email: String::from("another@example.com"),
        ..user1 // takes other fields from user1 ; can't use _user1_ anymore
    };
```
3. Кортежные структуры:
> Rust также поддерживает структуры, похожие на кортежи, которые называются кортежные структуры. Кортежные структуры обладают дополнительным смыслом, который даёт имя структуры, но при этом не имеют имён, связанных с их полями. Скорее, они просто хранят типы полей. Кортежные структуры полезны, когда вы хотите дать имя всему кортежу и сделать кортеж отличным от других кортежей, и когда именование каждого поля, как в обычной структуре, было бы многословным или избыточным.

```rust
struct Color(i32, i32, i32);
struct Point(i32, i32, i32);

fn main() {
    let black = Color(0, 0, 0);
    let origin = Point(0, 0, 0);
}
```
4. Единично-подобные структуры
> Также можно определять структуры, не имеющие полей! Они называются единично-подобными структурами, поскольку ведут себя аналогично (), единичному типу, о котором мы говорили в разделе "Тип кортежа". Единично-подобные структуры могут быть полезны, когда требуется реализовать типаж для некоторого типа, но у вас нет данных, которые нужно хранить в самом типе. Мы обсудим типажи в главе 10. Вот пример объявления и создание экземпляра единичной структуры с именем AlwaysEqual:

```rust
struct AlwaysEqual;

fn main() {
    let subject = AlwaysEqual;
}
```
### Перечисления
#### ключевое слово `enum`.
1. Инициализация
```rust 
enum IpAddrKind {
    V4,
    V6,
}
```
2. Использование 
```rust
enum IpAddrKind {
        V4,
        V6,
    }

    struct IpAddr {
        kind: IpAddrKind,
        address: String,
    }

    let home = IpAddr {
        kind: IpAddrKind::V4,
        address: String::from("127.0.0.1"),
    };

    let loopback = IpAddr {
        kind: IpAddrKind::V6,
        address: String::from("::1"),
    };
    ///////////////////////////////////////
    enum IpAddr {
        V4(String),
        V6(String),
    }

    let home = IpAddr::V4(String::from("127.0.0.1"));
    let loopback = IpAddr::V6(String::from("::1"));
    //////////////////////////////////////
    enum IpAddr {
        V4(u8, u8, u8, u8),
        V6(String),
    }

    let home = IpAddr::V4(127, 0, 0, 1);
```
3. с особоым типом внутри
```rust
    enum Message {
        Quit,
        Move { x: i32, y: i32 },
        Write(String),
        ChangeColor(i32, i32, i32),
    }
```
- Quit - пустой элемент без ассоциированных данных,
- Move имеет именованные поля, как и структура.
- Write - элемент с единственной строкой типа String,
- ChangeColor - кортеж из трёх значений типа i32.
4. Сходства со структурами
```rust 
    impl Message {
        fn call(&self) {
            // method body would be defined here
        }
    }

    let m = Message::Write(String::from("hello"));
    m.call();
```
> Есть ещё одно сходство между перечислениями и структурами: так же, как мы можем определять методы для структур с помощью impl блока, мы можем определять и методы для перечисления. Вот пример метода с именем call, который мы могли бы определить в нашем перечислении Message
5. Match
#### Условно похож на `switch`. Ищет вхождения в объекте, если находит - вызывает событие после `=>`
```rust 
    match coin {
        Coin::Penny => 1,
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter => 25,
    }
```
#### Если coin == Coin::Penny, то вернется `1`.
> Думайте о выражении match как о машине для сортировки монет: монеты скользят вниз по дорожке с отверстиями разного размера и каждая монета проваливается в первое отверстие, в которое она проходит. Таким же образом значения проходят через каждый шаблон в конструкции match и при первом же совпадении с шаблоном значение "проваливается" в соответствующий блок кода для дальнейшего использования.
```rust
enum Coin {
    Penny,
    Nickel,
    Dime,
    Quarter,
}

fn value_in_cents(coin: Coin) -> u8 {
    match coin {
        Coin::Penny => 1,
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter => 25,
    }
}
```
6. Возможность использования выражений при использовании `Match`.
> Фигурные скобки обычно не используются, если код ветки короткий, как в листинге 6-3, где каждая ветка только возвращает значение. Если необходимо выполнить несколько строк кода в ветке, можно использовать фигурные скобки. Например, следующий код будет выводить «Lucky penny!» каждый раз, когда метод вызывается со значением Coin::Penny, но возвращаться при этом будет результат последнего выражения в блоке, то есть значение 1
```rust
fn value_in_cents(coin: Coin) -> u8 {
    match coin {
        Coin::Penny => {
            println!("Lucky penny!");
            1
        }
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter => 25,
    }
}
```
7. Шаблоны, которые привязывают значения
```rust
#[derive(Debug)] // so we can inspect the state in a minute
enum UsState {
    Alabama,
    Alaska,
    // --snip--
}

enum Coin {
    Penny,
    Nickel,
    Dime,
    Quarter(UsState),
}
```
> В выражении match для этого кода мы добавляем переменную с именем state в шаблон, который соответствует значениям варианта Coin::Quarter. Когда Coin::Quarter совпадёт с шаблоном, переменная state будет привязана к значению штата этого четвертака. Затем мы сможем использовать state в коде этой ветки, вот так:
```rust
fn value_in_cents(coin: Coin) -> u8 {
    match coin {
        Coin::Penny => 1,
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter(state) => {
            println!("State quarter from {:?}!", state);
            25
        }
    }
}
```
> Если мы сделаем вызов функции value_in_cents(Coin::Quarter(UsState::Alaska)), то coin будет иметь значение Coin::Quarter(UsState::Alaska). Когда мы будем сравнивать это значение с каждой из веток, ни одна из них не будет совпадать, пока мы не достигнем варианта Coin::Quarter(state). В этот момент state привяжется к значению UsState::Alaska. Затем мы сможем использовать эту привязку в выражении println!, получив таким образом внутреннее значение варианта Quarter перечисления Coin.
8. Сопоставление шаблона для Option<T>
```rust
 fn plus_one(x: Option<i32>) -> Option<i32> {
        match x {
            None => None,
            Some(i) => Some(i + 1),
        }
    }

    let five = Some(5);
    let six = plus_one(five);
    let none = plus_one(None);
```
9. Заполнитель `_`
> В Rust также есть шаблон, который можно использовать, когда не хочется перечислять все возможные значения. Например, u8 может иметь допустимые значения от 0 до 255. Если мы только заботимся о значениях 1, 3, 5 и 7 и не хотим перечислять 0, 2, 4, 6, 8, 9 вплоть до 255, то к счастью нам это не нужно. Вместо этого можно использовать специальный шаблон `_`
10. if let
> Синтаксис if let позволяет скомбинировать if и let в менее многословную конструкцию, и затем обработать значения соответствующе только одному шаблону, одновременно игнорируя все остальные. Рассмотрим программу, в которой мы делаем поиск по шаблону значения Option<u8>, чтобы выполнить код только когда значение равно 3:
```rust 
    let config_max = Some(3u8);
    match config_max {
        Some(max) => println!("The maximum is configured to be {}", max),
        _ => (),
    }
    /////////////////////////////////////////////////////////////////////
    // Эквивалентный код
    let config_max = Some(3u8);
    if let Some(max) = config_max {
        println!("The maximum is configured to be {}", max);
    }
```
> Можно добавлять else к if let. Блок кода, который находится внутри else аналогичен по смыслу блоку кода ветки связанной с шаблоном _ выражения match (которое эквивалентно сборной конструкции if let и else).
```rust
    let mut count = 0;
    match coin {
        Coin::Quarter(state) => println!("State quarter from {:?}!", state),
        _ => count += 1,
    }
    /////// Эквивалентно
    let mut count = 0;
    if let Coin::Quarter(state) = coin {
        println!("State quarter from {:?}!", state);
    } else {
        count += 1;
    }
```

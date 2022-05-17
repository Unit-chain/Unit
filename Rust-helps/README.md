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
### Модули (`cargo new --lib restaurant`)
#### Ключевое слово `mod`.
> Создадим новую библиотеку (библиотечный крейт) с именем `restaurant` выполнив команду `cargo new --lib restaurant`
```rust
   mod front_of_house {
        mod hosting {
            fn add_to_waitlist() {}

            fn seat_at_table() {}
        }

        mod serving {
            fn take_order() {}

            fn serve_order() {}

            fn take_payment() {}
        }
    }

 
```    
#### Ссылаемся на элементы дерева модулей при помощи путей
```rust
mod front_of_house {
    mod hosting {
        fn add_to_waitlist() {}
    }
}

pub fn eat_at_restaurant() {
    // Absolute path
    crate::front_of_house::hosting::add_to_waitlist();

    // Relative path
    front_of_house::hosting::add_to_waitlist();
}
```
#### Super
```rust
fn deliver_order() {}

mod back_of_house {
    fn fix_incorrect_order() {
        cook_order();
        super::deliver_order();
    }

    fn cook_order() {}
}

```
> `super`обращается к родительскому модулю для `back_of_house`. Т.к родительский модуль для `back_of_house` - `crate`, то это эквивалентно: `crate::deliver_order()`

#### Структуры в модуле
```rust
mod back_of_house {
    pub struct Breakfast {
        pub toast: String,
        seasonal_fruit: String,
    }

    impl Breakfast {
        pub fn summer(toast: &str) -> Breakfast {
            Breakfast {
                toast: String::from(toast),
                seasonal_fruit: String::from("peaches"),
            }
        }
    }
}

pub fn eat_at_restaurant() {
    // Order a breakfast in the summer with Rye toast
    let mut meal = back_of_house::Breakfast::summer("Rye");
    // Change our mind about what bread we'd like
    meal.toast = String::from("Wheat");
    println!("I'd like {} toast please", meal.toast);

    // The next line won't compile if we uncomment it; we're not allowed
    // to see or modify the seasonal fruit that comes with the meal
    // meal.seasonal_fruit = String::from("blueberries");
}
```    
#### Enum
```rust
mod back_of_house {
    pub enum Appetizer {
        Soup,
        Salad,
    }
}

pub fn eat_at_restaurant() {
    let order1 = back_of_house::Appetizer::Soup;
    let order2 = back_of_house::Appetizer::Salad;
}
```
> Поскольку мы сделали публичным список Appetizer, то можно использовать варианты Soup и Salad в функции eat_at_restaurant. Перечисления не очень полезны, если их варианты являются приватными: было бы досадно каждый раз аннотировать все перечисленные варианты как pub. По этой причине по умолчанию варианты перечислений являются публичными. Структуры часто полезны, если их поля не являются открытыми, поэтому поля структуры следуют общему правилу, согласно которому всё по умолчанию является приватными, если не указано pub.

#### Use
```rust
mod front_of_house {
    pub mod hosting {
        pub fn add_to_waitlist() {}
    }
}

use crate::front_of_house::hosting;

pub fn eat_at_restaurant() {
    hosting::add_to_waitlist();
}
```
> `use` в *Rust* эквивалентно `using` в `C/C++` 
```c++
#include <iostream>
using namespace std;
int main() {
    cout << "Hello, world!";
}
```
> без `using` пришлось бы писать длиннее:
```c++
#include <iostream>
int main() {
    std::cout << "Hello, world!";
}
```
> `use` - указывает область видимости
#### As
```rust
use std::fmt::Result;
use std::io::Result as IoResult;

fn function1() -> Result {
    // --snip--
}

fn function2() -> IoResult<()> {
    // --snip--
}
```
> `As` позволяет избежать ошибок, связанных с одниковым названием модулей/методов в разных библиблиотеках/модулях

#### Реэкспорт имён с pub use
```rust
pub use crate::front_of_house::hosting;
```
> Благодаря использованию pub use, внешний код теперь может вызывать функцию add_to_waitlist используя hosting::add_to_waitlist. Если бы мы не указали pub use, то только функция eat_at_restaurant могла бы вызывать hosting::add_to_waitlist в своей области видимости, но внешний код не смог бы так сделать.
> Вместо подключение двух модулей из одной облости видимости в две строки, можно объединить в одну 
```rust
// --snip--
use std::cmp::Ordering;
use std::io;
// --snip--
    /// eqauls
// --snip--
use std::{cmp::Ordering, io};
// --snip--
use std::io::{self, Write};
```
> Для подключения всех модулей из пакета, можно использовать оператор `*`
```rust
use std::collections::*;
```
#### Разделение модулей на разные файлы
```rust
mod front_of_house;

pub use crate::front_of_house::hosting;

pub fn eat_at_restaurant() {
    hosting::add_to_waitlist();
}
```
> Например, давайте начнём с кода листинга 7-17 и первым шагом переместим модуль front_of_house в свой собственный файл src/front_of_house.rs, изменив корневой файл крейта так, чтобы он содержал код показанный в листинге 7-21. В этом случае, корневым файлом крейта является src/lib.rs, но эта процедура также работает с исполняемыми крейтами у которых корневой файл крейта src/main.rs.
> В данном случае будет искать файл с именем как у модуля, то есть `src/front_of_house.rs`. 
> Инициализируем тело `hosting` в `front_of_house.rs`
```rust
pub mod hosting {
    pub fn add_to_waitlist() {}
}
```
> Использование точки с запятой после mod front_of_house, вместо объявления начала блока, говорит Rust загрузить содержимое модуля из другого файла имеющего такое же название как и имя модуля. Продолжим наш пример и выделим модуль hosting в отдельный файл, а затем поменяем содержимое файла src/front_of_house.rs так, чтобы он содержал только объявление модуля hosting:
```rust
pub mod hosting;
```
> Затем мы создаём каталог src/front_of_house и файл src/front_of_house/hosting.rs в данной директории. Чтобы вынести модуль мы, так же как и ранее, должны выделить содержимое модуля hosting из прежнего места и перенести его в свой файл модуля hosting.rs:
> Файл: src/front_of_house/hosting.rs
```rust
pub fn add_to_waitlist() {}
```    
> это эквивалентно тому, что мы бы описали модуль в src/lib.rs

### Коллекции
#### Создание нового вектора
```rust
    let v: Vec<i32> = Vec::new();
//// or
    let v = vec![1, 2, 3]; // Vec<i32>
```
> Добавление элементов
```rust
    let mut v = Vec::new();
    v.push(5);
    v.push(6);
    v.push(7);
    v.push(8);
```
> Удаление вектора происходит при выходе из области видимсоти    
    
> Способы сослаться на значение
```rust
    let v = vec![1, 2, 3, 4, 5];

    let third: &i32 = &v[2];
    println!("The third element is {}", third);

    match v.get(2) {
        Some(third) => println!("The third element is {}", third),
        None => println!("There is no third element."),
    }
```
> Можно так же итерировать ссылки
```rust
    let mut v = vec![100, 32, 57];
    for i in &mut v {
        *i += 50;
    }
```
#### Использование перечислений для хранения множества разных типов
> Векторы могут хранить значения только одинакового типа. Это может быть неудобно; определённо есть варианты использования для хранения списка элементов разных типов. К счастью, варианты перечисления определены для одного и того же типа перечисления, поэтому, когда нам нужен один тип для представления элементов разных типов, мы можем определить и использовать перечисление!
> Например, мы хотим получить значения из строки в электронной таблице где некоторые столбцы строки содержат целые числа, некоторые числа с плавающей точкой, а другие - строковые значения. Можно определить перечисление, варианты которого будут содержать разные типы значений и тогда все варианты перечисления будут считаться одними и тем же типом: типом самого перечисления. Затем можно создать вектор, который содержит в себе значения этого перечисления и таким образом в конечном счёте добиться того, что в векторе будут сохраняться значения разного типа. Мы продемонстрировали это в листинге 8-10.
```rust
    enum SpreadsheetCell {
        Int(i32),
        Float(f64),
        Text(String),
    }

    let row = vec![
        SpreadsheetCell::Int(3),
        SpreadsheetCell::Text(String::from("blue")),
        SpreadsheetCell::Float(10.12),
    ];
```
#### Строки
> Присоединение к строке с помощью push_str и push
```rust 
    let mut s = String::from("foo");
    s.push_str("bar"); // foobar
```
> Метод `push` принимает один символ в качестве параметра и добавляет его к String. В листинге 8-17 показан код, добавляющий букву "l" к String, используя метод push.
```rust
    let mut s = String::from("lo");
    s.push('l');
```
> Объединение строк с помощью оператора + или макроса format!
```rust
    let s1 = String::from("Hello, ");
    let s2 = String::from("world!");
    let s3 = s1 + &s2; // note s1 has been moved here and can no longer be used
```
> Макрос `format`
```rust
    let s1 = String::from("tic");
    let s2 = String::from("tac");
    let s3 = String::from("toe");
    let s = format!("{}-{}-{}", s1, s2, s3); // tic-tac-toe
```
> Получение буквы из строки
```rust
    let hello = "Здравствуйте";
    let answer = &hello[0];
```
> Срезы
```rust
let hello = "Здравствуйте"; // Each symbol is 2 bytes. Remember it, cause first symbol can't be get by 1 byte
let s = &hello[0..4]; // from zero to fourth byte. It's "Зд"
```
> Методы перебора
```rust
for c in "नमस्ते".chars() {
    println!("{}", c);
}
// न
// म
// स
// ्
// त
// े
    
for b in "नमस्ते".bytes() {
    println!("{}", b);
}
// 164
// --часть байтов вырезана--
// 165
// 135 
```
#### HashMap
`HashMap<K, V>`, K - Key, V - Value
> Создание новой хэш-карты
```rust
    use std::collections::HashMap;

    let mut scores = HashMap::new(); // String : i32
    scores.insert(String::from("Blue"), 10);
    scores.insert(String::from("Yellow"), 50);
```
> Ещё один способ построения хеш-карты - использование метода collect на векторе кортежей, где каждый кортеж состоит из двух значений (первое может быть представлено как ключ, а второе как значение хеш-карты). Метод collect собирает данные в несколько типов коллекций, включая HashMap . Например, если бы у нас были названия команд и начальные результаты в двух отдельных векторах, то мы могли бы использовать метод zip для создания вектора кортежей, где имя "Blue" спарено с числом 10, и так далее. Тогда мы могли бы использовать метод collect, чтобы превратить этот вектор кортежей в HashMap, как показано в листинге 8-21.
```rust
    use std::collections::HashMap;

    let teams = vec![String::from("Blue"), String::from("Yellow")];
    let initial_scores = vec![10, 50];

    let mut scores: HashMap<_, _> =
        teams.into_iter().zip(initial_scores.into_iter()).collect(); // String(teams) : i32(initial_scores)
```
> Для типов, которые реализуют типаж Copy, например i32, значения копируются в HashMap. Для значений со владением, таких как String, значения будут перемещены в хеш-карту и она станет владельцем этих значений, как показано в листинге 8-22.
```rust
    use std::collections::HashMap;

    let field_name = String::from("Favorite color");
    let field_value = String::from("Blue");

    let mut map = HashMap::new();
    map.insert(field_name, field_value);
    // field_name and field_value are invalid at this point, try using them and
    // see what compiler error you get!
```

> Мы не можем использовать переменные field_name и field_value после того, как их значения были перемещены в HashMap вызовом метода insert.
> Если мы вставим в HashMap ссылки на значения, то они не будут перемещены в HashMap. Значения, на которые указывают ссылки, должны быть действительными хотя бы до тех пор, пока хеш-карта действительна.
##### Получение значеня
```rust
    use std::collections::HashMap;

    let mut scores = HashMap::new();

    scores.insert(String::from("Blue"), 10);
    scores.insert(String::from("Yellow"), 50);

    let team_name = String::from("Blue");
    let score = scores.get(&team_name);
```
> Мы можем перебирать каждую пару ключ/значение в HashMap таким же образом, как мы делали с векторами, используя цикл for:
```rust
    use std::collections::HashMap;

    let mut scores = HashMap::new();

    scores.insert(String::from("Blue"), 10);
    scores.insert(String::from("Yellow"), 50);

    for (key, value) in &scores {
        println!("{}: {}", key, value);
    }
```
> Результат: 
    
```
Yellow: 50
Blue: 10
```
# Перезапись старых значений
> Если мы вставим ключ и значение в HashMap, а затем вставим тот же ключ с новым значением, то старое значение связанное с этим ключом, будет заменено на новое. Даже несмотря на то, что код в листинге 8-24 вызывает insert дважды, хеш-карта будет содержать только одну пару ключ/значение, потому что мы вставляем значения для одного и того же ключа - ключа команды "Blue".
```rust
    use std::collections::HashMap;

    let mut scores = HashMap::new();

    scores.insert(String::from("Blue"), 10);
    scores.insert(String::from("Blue"), 25);

    println!("{:?}", scores); 
```
> Код напечатает `{"Blue": 25}`. Начальное значение `10` было перезаписано.

##### Вставка значения только в том случае, когда ключ не имеет значения
```rust
    use std::collections::HashMap;

    let mut scores = HashMap::new();
    scores.insert(String::from("Blue"), 10);

    scores.entry(String::from("Yellow")).or_insert(50);
    scores.entry(String::from("Blue")).or_insert(50);

    println!("{:?}", scores);
```
> Результат: 
    
`{"Yellow": 50, "Blue": 10}`
##### Создание нового значения на основе старого значения
> Другим распространённым вариантом использования хеш-карт является поиск значения по ключу, а затем обновление этого значения на основе старого значения. Например, в листинге 8-26 показан код, который подсчитывает, сколько раз определённое слово появляется в каком-либо тексте. Мы используем HashMap со словами в качестве ключей и увеличиваем соответствующее слову значение, чтобы отслеживать, сколько раз в тексте мы увидели слово. Если мы впервые увидели слово, то сначала вставляем значение 0.
```rust
    use std::collections::HashMap;

    let text = "hello world wonderful world";

    let mut map = HashMap::new();

    for word in text.split_whitespace() {
        let count = map.entry(word).or_insert(0);
        *count += 1;
    }

    println!("{:?}", map);
```
> Результат:
    
`{"world": 2, "hello": 1, "wonderful": 1}.`
> По умолчанию HashMap использует "криптографически сильную" функцию хэширования `SipHash`, которая может противостоять атакам класса отказ в обслуживании, Denial of Service (DoS). Это не самый быстрый из возможных алгоритмов хеширования, в данном случае производительность идёт на компромисс с обеспечением лучшей безопасности. Если после профилирования вашего кода окажется, что хэш функция используемая по умолчанию очень медленная, вы можете заменить её используя другой hasher. Hasher - это тип, реализующий трейт BuildHasher. Подробнее о типажах мы поговорим в Главе 10. Вам совсем не обязательно реализовывать свою собственную функцию хэширования, crates.io имеет достаточное количество библиотек, предоставляющих разные реализации hasher с множеством общих алгоритмов хэширования.
	# Обработка ошибок
> Большинство языков не различают эти два вида ошибок и обрабатывают оба вида одинаково, используя такие механизмы, как исключения. В Rust нет исключений. Вместо этого он имеет тип `Result<T, E>` для обрабатываемых (исправимых) ошибок и макрос `panic!`, который останавливает выполнение, когда программа встречает необрабатываемую (неисправимую) ошибку. Сначала эта глава расскажет про вызов `panic!`, а потом расскажет о возврате значений `Result<T, E>`. Кроме того, мы рассмотрим, что нужно учитывать при принятии решения о том, следует ли попытаться исправить ошибку или остановить выполнение.
### Раскручивать стек или прерывать выполнение программы в ответ на панику
> По умолчанию, когда происходит паника, программа начинает процесс _раскрутки стека_, означающий в Rust проход обратно по стеку вызовов и очистку данных для каждой обнаруженной функции. Тем не менее, этот обратный проход по стеку и очистка генерируют много работы. Альтернативой является немедленное _прерывание_выполнения, которое завершает программу без очистки. Память, которую использовала программа, должна быть очищена операционной системой. Если в вашем проекте нужно сделать маленьким исполняемый файл, насколько это возможно, вы можете переключиться с варианта раскрутки стека на вариант прерывания при панике, добавьте `panic = 'abort'` в раздел `[profile]` вашего _Cargo.toml_ файла. Например, если вы хотите прерывать выполнение программы по панике в релизной версии программы добавьте следующее:
```toml
[profile.release] 
panic = 'abort'
```
> Давайте посмотрим на другой пример, где, вызов `panic!` происходит в сторонней библиотеке из-за ошибки в нашем коде (а не как в примере ранее, из-за вызова макроса нашим кодом напрямую). В листинге 9-1 приведён код, который пытается получить доступ по индексу в векторе за пределами допустимого диапазона значений индекса.
```rust
fn main() {
	let v = vec![1, 2, 3]; v[99]; 
}
```	
> Здесь мы пытаемся получить доступ к 100-му элементу вектора (который находится по индексу 99, потому что индексирование начинается с нуля), но вектор имеет только 3 элемента. В этой ситуации, Rust будет вызывать панику. Использование `[]` должно возвращать элемент, но вы передаёте неверный индекс: не существует элемента, который Rust мог бы вернуть.
	
```markdown
	cargo run Compiling panic v0.1.0 (file:///projects/panic) Finished dev [unoptimized + debuginfo] target(s) in 0.27s Running `target/debug/panic` thread 'main' panicked at 'index out of bounds: the len is 3 but the index is 99', src/main.rs:4:5 note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace
````
> Следующая строка говорит, что мы можем установить переменную среды `RUST_BACKTRACE`, чтобы получить обратную трассировку того, что именно стало причиной ошибки. _Обратная трассировка_ создаёт список всех функций, которые были вызваны до какой-то определённой точки выполнения программы. Обратная трассировка в Rust работает так же, как и в других языках. По этому предлагаем вам читать данные обратной трассировки как и везде - читать сверху вниз, пока не увидите информацию о файлах написанных вами. Это место, где возникла проблема. Другие строки, которые выше над строками с упоминанием наших файлов, - это код, который вызывается нашим кодом; строки ниже являются кодом, который вызывает наш код. Эти строки могут включать основной код Rust, код стандартной библиотеки или используемые крейты. Давайте попробуем получить обратную трассировку с помощью установки переменной среды `RUST_BACKTRACE` в любое значение, кроме 0. Листинг 9-2 показывает вывод, подобный тому, что вы увидите.

```markdown
$ RUST_BACKTRACE=1 cargo run thread 'main' panicked at 'index out of bounds: the len is 3 but the index is 99', src/main.rs:4:5 stack backtrace: 0: rust_begin_unwind at /rustc/7eac88abb2e57e752f3302f02be5f3ce3d7adfb4/library/std/src/panicking.rs:483 1: core::panicking::panic_fmt at /rustc/7eac88abb2e57e752f3302f02be5f3ce3d7adfb4/library/core/src/panicking.rs:85 2: core::panicking::panic_bounds_check at /rustc/7eac88abb2e57e752f3302f02be5f3ce3d7adfb4/library/core/src/panicking.rs:62 3: <usize as core::slice::index::SliceIndex<[T]>>::index at /rustc/7eac88abb2e57e752f3302f02be5f3ce3d7adfb4/library/core/src/slice/index.rs:255 4: core::slice::index::<impl core::ops::index::Index<I> for [T]>::index at /rustc/7eac88abb2e57e752f3302f02be5f3ce3d7adfb4/library/core/src/slice/index.rs:15 5: <alloc::vec::Vec<T> as core::ops::index::Index<I>>::index at /rustc/7eac88abb2e57e752f3302f02be5f3ce3d7adfb4/library/alloc/src/vec.rs:1982 6: panic::main at ./src/main.rs:4 7: core::ops::function::FnOnce::call_once at /rustc/7eac88abb2e57e752f3302f02be5f3ce3d7adfb4/library/core/src/ops/function.rs:227 note: Some details are omitted, run with `RUST_BACKTRACE=full` for a verbose backtrace.
```	
# Исправимые ошибки с `Result<T, E>`.
> Само перечисление определено таким образом
```rust
enum Result<T, E> {
	Ok(T), 
	Err(E), 
}
```	

## Success
```rust
use std::fs::File; 
fn main() { 
	let f = File::open("hello.txt"); 
	let f = match f { 
		Ok(file) => file, 
		Err(error) => panic!("Problem opening the file: {:?}", error),
	};
}
```
## Обработка различных ошибок c помощью `match`
```rust
use std::fs::File;
use std::io::ErrorKind;

fn main() {
    let f = File::open("hello.txt");

    let f = match f {
        Ok(file) => file,
        Err(error) => match error.kind() {
            ErrorKind::NotFound => match File::create("hello.txt") {
                Ok(fc) => fc,
                Err(e) => panic!("Problem creating the file: {:?}", e),
            },
            other_error => {
                panic!("Problem opening the file: {:?}", other_error)
            }
        },
    };
}

```
> Типом значения возвращаемого функцией `File::open` внутри `Err` варианта является `io::Error`, структура из стандартной библиотеки. Данная структура имеет метод `kind`, который можно вызвать для получения значения `io::ErrorKind`. Перечисление `io::ErrorKind` из стандартной библиотеки имеет варианты, представляющие различные типы ошибок, которые могут появиться при выполнении операций в `io` (крейте который занимается проблемами ввода/вывода данных). Вариант, который мы хотим использовать, это `ErrorKind::NotFound`. Он даёт информацию, о том, что файл который мы пытаемся открыть ещё не существует. Итак, во второй строке мы вызываем сопоставление шаблона с переменной `f` и попадаем в ветку с обработкой ошибки, но также у нас есть внутренняя проверка для сопоставления `error.kind()` ошибки.

> Условие, которое мы хотим проверить во внутреннем `match` - это то, что значение, которое вернул вызов `error.kind()`, является вариантом `NotFound` перечисления `ErrorKind`. Если это так, мы пытаемся создать файл с помощью функции `File::create`. Однако, поскольку вызов `File::create` тоже может завершиться ошибкой, нам нужна обработка ещё одной ошибки теперь уже во внутреннем выражении `match` - третий вложенный `match`. Заметьте: если файл не может быть создан, выводится другое сообщение об ошибке, более специализированное. Вторая же ветка внешнего `match` (который обрабатывает вызов `error.kind()`), остаётся той же самой. В итоге программа паникует при любой ошибке, кроме ошибки отсутствия файла.	
	
## Сокращённые способы обработки ошибок `unwrap` и `expect`
> Использование `match` работает неплохо, однако может выглядеть несколько многословно и не всегда хорошо передаёт намерения. У типа `Result<T, E>` есть много методов для различных задач. Один из них, `unwrap`, является сокращённым методом, который реализован прямо как выражение `match` из листинга 9-4. Если значение `Result` это `Ok`, `unwrap` вернёт значение внутри `Ok`. Если же `Result` это `Err`, `unwrap` вызовет макрос `panic!`. Вот пример `unwrap` в действии:
	
```rust
use std::fs::File;

fn main() {
    let f = File::open("hello.txt").unwrap();
}
```
> Ошибка:

```markdown
thread 'main' panicked at 'called `Result::unwrap()` on an `Err` value: Error { repr: Os { code: 2, message: "No such file or directory" } }', src/libcore/result.rs:906:4
```
	
## Expect
> Другой метод, похожий на `unwrap`, это `expect`, позволяющий выбрать сообщение об ошибке для макроса `panic!`. Использование `expect` вместо `unwrap` с предоставлением хорошего сообщения об ошибке выражает ваше намерение и делает более простым отслеживание источника паники. Синтаксис метода `expect` выглядит так:
	
```rust
use std::fs::File;

fn main() {
    let f = File::open("hello.txt").expect("Failed to open hello.txt");
}

```

## Проброс ошибок

> Когда вы пишете функцию, реализация которой вызывает что-то, что может завершиться ошибкой, вместо обработки ошибки в этой функции, вы можете вернуть ошибку в вызывающий код, чтобы он мог решить, что с ней делать. Такой приём известен как _распространение ошибки, propagating the error_. Благодаря нему мы даём больше контроля вызывающему коду, где может быть больше информации или логики, которая диктует, как ошибка должна обрабатываться, чем было бы в месте появления этой ошибки.
> Например, код программы 9-6 читает имя пользователя из файла. Если файл не существует или не может быть прочтён, то функция возвращает ошибку в код, который вызвал данную функцию:
	
```rust

#![allow(unused)]
fn main() {
use std::fs::File;
use std::io::{self, Read};

	fn read_username_from_file() -> Result<String, io::Error> {
		let f = File::open("hello.txt");

		let mut f = match f {
			Ok(file) => file,
			Err(e) => return Err(e),
		};

		let mut s = String::new();

		match f.read_to_string(&mut s) {
			Ok(_) => Ok(s),
			Err(e) => Err(e),
		}
	}
}
```
## Сокращение для проброса ошибок: оператор `?`
```rust
#![allow(unused)]
fn main() {
use std::fs::File;
use std::io;
use std::io::Read;

	fn read_username_from_file() -> Result<String, io::Error> {
		let mut f = File::open("hello.txt")?;
		let mut s = String::new();
		f.read_to_string(&mut s)?;
		Ok(s)
	}
}
```

> Оператор `?`, помещаемый после значения типа `Result`, работает почти таким же образом, как выражение `match`, которое мы определили для обработки значений типа `Result` в листинге 9-6. Если значение `Result` равно `Ok`, значение внутри `Ok` будет возвращено из этого выражения и программа продолжит выполнение. Если значение является `Err`, то `Err`будет возвращено из всей функции, как если бы мы использовали ключевое слово `return`, таким образом ошибка передаётся в вызывающий код.

> Использование оператора `?` позволят уменьшить количество строк кода и сделать реализацию проще. Написанный в предыдущем примере код можно сделать ещё короче с помощью сокращения промежуточных переменных и конвейерного вызова нескольких методов подряд, как показано в листинге 9-8:
	
```rust

#![allow(unused)]
fn main() {
use std::fs::File;
use std::io;
use std::io::Read;

	fn read_username_from_file() -> Result<String, io::Error> {
		let mut s = String::new();
		File::open("hello.txt")?.read_to_string(&mut s)?;
		Ok(s)
	}
}
```

> Мы перенесли в начало функции создание новой переменной `s` типа `String`; эта часть не изменилась. Вместо создания переменной `f` мы добавили вызов `read_to_string`непосредственно к результату `File::open("hello.txt")?`, У нас ещё есть `?` в конце вызова `read_to_string`, и мы по-прежнему возвращаем значение `Ok`, содержащее имя пользователя в `s` когда оба метода: `File::open` и `read_to_string` успешны, а не возвращают ошибки. Функциональность снова такая же, как в листинге 9-6 и листинге 9-7; это просто другой, более эргономичный способ решения той же задачи.
> Продолжая рассматривать разные способы записи данной функции, листинг 9-9 показывает способ сделать её ещё короче.

```rust
#![allow(unused)]
fn main() {
use std::fs;
use std::io;
	fn read_username_from_file() -> Result<String, io::Error> {
		fs::read_to_string("hello.txt")
	}
}
```

> Использование `?` допустимо только для `Result` и `Option`
[Boxing errors](https://doc.rust-lang.org/stable/rust-by-example/error/multiple_error_types/boxing_errors.html)
	
# Обобщенные типы
> 10.1 как  в Java

## Типажи
> Примечание: Типажи похожи на функциональность часто называемую _интерфейсами_ в других языках, хотя и с некоторыми отличиями.
	
 Мы хотим создать библиотеку медиа-агрегатора, которая может отображать сводку данных сохранённых в экземплярах структур `NewsArticle` или `Tweet`. Чтобы этого достичь, нам необходимо иметь возможность для каждой структуры сделать короткую сводку на основе имеющихся данных: надо, чтобы обе структуры реализовали общее поведение. Мы можем делать такую сводку вызовом метода `summarize` у экземпляра объекта. Пример листинга 10-12 иллюстрирует определение типажа `Summary`, который выражает данное поведение:

```rust
pub trait Summary {
    fn summarize(&self) -> String;
}
```

Здесь мы объявляем типаж с использованием ключевого слова `trait`, а затем его название, которым является `Summary` в данном случае. Внутри фигурных скобок объявляются сигнатуры методов, которые описывают поведения типов, реализующих данный типаж, в данном случае поведение определяется только одной сигнатурой метода: `fn summarize(&self) -> String`.

После сигнатуры метода, вместо предоставления реализации в фигурных в скобках, мы используем точку с запятой. Каждый тип, реализующий данный типаж, должен предоставить своё собственное поведение для данного метода. Компилятор обеспечит, что любой тип содержащий типаж `Summary`, будет также иметь и метод `summarize` объявленный с точно такой же сигнатурой.

Типаж может иметь несколько методов в описании его тела: сигнатуры методов перечисляются по одной на каждой строке и должны закачиваться символом `;`.

## Реализация типажа у типа
	
Теперь, после того как мы определили желаемое поведение используя типаж `Summary`, можно реализовать его у типов в нашем медиа-агрегаторе. Листинг 10-13 показывает реализацию типажа `Summary` у структуры `NewsArticle`, которая использует для создания сводки в методе `summarize` заголовок, автора и место публикации статьи. Для структуры `Tweet` мы определяем реализацию `summarize` используя пользователя и полный текст твита, полагая содержание твита уже ограниченным 280 символами.

```rust
pub struct NewsArticle {
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}

impl Summary for NewsArticle {
    fn summarize(&self) -> String {
        format!("{}, by {} ({})", self.headline, self.author, self.location)
    }
}

pub struct Tweet {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}

impl Summary for Tweet {
    fn summarize(&self) -> String {
        format!("{}: {}", self.username, self.content)
    }
}
```	
	
Реализация типажа у типа аналогична реализации обычных методов. Разница в том, что после `impl` мы ставим имя типажа, который мы хотим реализовать, затем используем ключевое слово `for`, а затем указываем имя типа, для которого мы хотим сделать реализацию типажа. Внутри блока `impl` мы помещаем сигнатуру метода объявленную в типаже. Вместо добавления точки с запятой в конце, после каждой сигнатуры используются фигурные скобки и тело метода заполняется конкретным поведением, которое мы хотим получить у методов типажа для конкретного типа.

Обратите внимание, что поскольку мы определили типаж `Summary` и типы `NewsArticle` и `Tweet` в одном и том же файле _lib.rs_ примера 10-13, все они находятся в одной области видимости. Допустим, что _lib.rs_ предназначен для крейта, который мы назвали `aggregator` и кто-то ещё хочет использовать функциональность нашего крейта для реализации типажа `Summary` у структуры, определённой в области видимости внутри их библиотеки. Им нужно будет сначала подключить типаж в их область видимости. Они сделали бы это, указав `use aggregator::Summary;`, что позволит реализовать `Summary` для их типа. Типажу `Summary` также необходимо быть публичным для реализации в других крейтах, потому мы поставили ключевое слово `pub` перед `trait` в листинге 10-12.

## Реализация поведения по умолчанию
```rust
pub trait Summary {
    fn summarize(&self) -> String {
        String::from("(Read more...)")
    }
}

pub struct NewsArticle {
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}

impl Summary for NewsArticle {}

pub struct Tweet {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}

impl Summary for Tweet {
    fn summarize(&self) -> String {
        format!("{}: {}", self.username, self.content)
    }
}
```
Для использования реализации по умолчанию при создании сводки у экземпляров `NewsArticle` вместо определения пользовательской реализации, мы указываем пустой блок `impl` с `impl Summary for NewsArticle {}`.

Хотя мы больше не определяем метод `summarize` непосредственно в `NewsArticle`, мы предоставили реализацию по умолчанию и указали, что `NewsArticle` реализует типаж `Summary`. В результате мы всё ещё можем вызвать метод `summarize` у экземпляра `NewsArticle`, например так:
```rust
use chapter10::{self, NewsArticle, Summary};

fn main() {
    let article = NewsArticle {
        headline: String::from("Penguins win the Stanley Cup Championship!"),
        location: String::from("Pittsburgh, PA, USA"),
        author: String::from("Iceburgh"),
        content: String::from(
            "The Pittsburgh Penguins once again are the best \
             hockey team in the NHL.",
        ),
    };

    println!("New article available! {}", article.summarize());
}
```
Создание реализации по умолчанию для метода `summarize` не требует от нас изменений чего-либо в реализации `Summary` для типа `Tweet` в листинге 10-13. Причина заключается в том, что синтаксис для переопределения реализации по умолчанию является таким же, как синтаксис для реализации метода типажа, который не имеет реализации по умолчанию.

Реализации по умолчанию могут вызывать другие методы в том же типаже, даже если эти другие методы не имеют реализации по умолчанию. Таким образом, типаж может предоставить много полезной функциональности и только требует от разработчиков указывать небольшую его часть. Например, мы могли бы определить типаж `Summary`имеющий метод `summarize_author`, реализация которого требуется, а затем определить метод `summarize` который имеет реализацию по умолчанию, которая внутри вызывает метод `summarize_author` :

```rust
pub trait Summary {
    fn summarize_author(&self) -> String;

    fn summarize(&self) -> String {
        format!("(Read more from {}...)", self.summarize_author())
    }
}

pub struct Tweet {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}

impl Summary for Tweet {
    fn summarize_author(&self) -> String {
        format!("@{}", self.username)
    }
}
```

Чтобы использовать такую версию типажа `Summary`, нужно только определить метод `summarize_author`, при реализации типажа для типа:
```rust
pub trait Summary {
    fn summarize_author(&self) -> String;

    fn summarize(&self) -> String {
        format!("(Read more from {}...)", self.summarize_author())
    }
}

pub struct Tweet {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}

impl Summary for Tweet {
    fn summarize_author(&self) -> String {
        format!("@{}", self.username)
    }
}
```
После того, как мы определим `summarize_author`, можно вызвать `summarize` для экземпляров структуры `Tweet` и реализация по умолчанию метода `summarize` будет вызывать определение `summarize_author` которое мы уже предоставили. Так как мы реализовали метод `summarize_author` типажа `Summary`, то типаж даёт нам поведение метода `summarize` без необходимости писать код.

```rust
use chapter10::{self, Summary, Tweet};

fn main() {
    let tweet = Tweet {
        username: String::from("horse_ebooks"),
        content: String::from(
            "of course, as you probably already know, people",
        ),
        reply: false,
        retweet: false,
    };

    println!("1 new tweet: {}", tweet.summarize());
}
```
	
## Типажи как параметры

Например, в листинге 10-13 мы реализовали типаж `Summary` для типов структур `NewsArticle` и `Tweet`. Можно определить функцию `notify` которая вызывает метод `summarize` с параметром `item`, который имеет тип реализующий типаж `Summary` . Для этого можно использовать синтаксис `&impl Trait`, например так:

```rust
pub trait Summary {
    fn summarize(&self) -> String;
}

pub struct NewsArticle {
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}

impl Summary for NewsArticle {
    fn summarize(&self) -> String {
        format!("{}, by {} ({})", self.headline, self.author, self.location)
    }
}

pub struct Tweet {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}

impl Summary for Tweet {
    fn summarize(&self) -> String {
        format!("{}: {}", self.username, self.content)
    }
}

pub fn notify(item: &impl Summary) {
    println!("Breaking news! {}", item.summarize());
}
```

Вместо конкретного типа у параметра `item` указывается ключевое слово `impl` и имя типажа. Этот параметр принимает любой тип, который реализует указанный типаж. В теле `notify` мы можем вызывать любые методы у экземпляра `item`, которые должны быть определены при реализации типажа `Summary`, например можно вызвать метод `summarize`. Мы можем вызвать `notify` и передать в него любой экземпляр `NewsArticle` или `Tweet`. Код, который вызывает данную функцию с любым другим типом, таким как `String` или `i32`, не будет компилироваться, потому что эти типы не реализуют типаж `Summary`.

### Синтаксис ограничения типажа
Синтаксис `impl Trait` работает для простых случаев, но на самом деле является синтаксическим сахаром для более длинной формы, которая называется _ограничением типажа_; это выглядит так:
```rust
pub fn notify<T: Summary>(item: &T) {
    println!("Breaking news! {}", item.summarize());
}
```
Синтаксис `impl Trait` удобен и делает более выразительным код в простых случаях. Синтаксис ограничений типажа может выразить большую сложность в других случаях. Например, у нас может быть два параметра, которые реализуют типаж `Summary`. Использование синтаксиса `impl Trait` выглядит следующим образом:
```rust
pub fn notify(item1: &impl Summary, item2: &impl Summary) {
```
Если бы мы хотели, чтобы эта функция позволяла иметь `item1` и `item2` разных типов, то использование `impl Trait` было бы уместно (до тех пор, пока оба типа реализуют `Summary`). Если мы хотим форсировать, чтобы оба параметра имели одинаковый тип, то это можно выразить только с использованием ограничения типажа, например так:
```rust
pub fn notify<T: Summary>(item1: &T, item2: &T) {
```
Обобщённый тип `T` указан для типов параметров `item1` и `item2` и ограничивает функцию так, что конкретные значения типов переданные аргументами в  `item1` и `item2` должны быть одинаковыми.
	
### Задание нескольких границ типажей с помощью синтаксиса `+`
	
Также можно указать более одного ограничения типажа. Скажем, мы хотели бы использовать в методе `notify` для параметра `item` с форматированием отображения, также как метод `summarize`: для этого мы указываем в определении `notify`, что `item` должен реализовывать как типаж `Display` так и `Summary`. Мы можем сделать это используя синтаксис `+`:
```rust
pub fn notify(item: &(impl Summary + Display)) {
```
Синтаксис `+` также допустим с ограничениями типажа для обобщённых типов:
```rust
pub fn notify<T: Summary + Display>(item: &T) {
```
### Более ясные границы типажа с помощью `where`
Использование слишком большого количества ограничений типажа имеет свои недостатки. Каждый обобщённый тип имеет свои границы типажа, поэтому функции с несколькими параметрами обобщённого типа могут содержать много информации об ограничениях между названием функции и списком её параметров затрудняющих чтение сигнатуры. По этой причине в Rust есть альтернативный синтаксис для определения ограничений типажа внутри предложения `where` после сигнатуры функции. Поэтому вместо того, чтобы писать так:
```rust
fn some_function<T: Display + Clone, U: Clone + Debug>(t: &T, u: &U) -> i32 {
```
можно использовать предложение `where` , например так:
```rust
fn some_function<T, U>(t: &T, u: &U) -> i32
    where T: Display + Clone,
          U: Clone + Debug
{
```

### Возврат значений типа реализующего определённый типаж

Также можно использовать синтаксис `impl Trait` в возвращаемой позиции, чтобы вернуть значение некоторого типа реализующего типаж, как показано здесь:

```rust
pub trait Summary {
    fn summarize(&self) -> String;
}

pub struct NewsArticle {
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}

impl Summary for NewsArticle {
    fn summarize(&self) -> String {
        format!("{}, by {} ({})", self.headline, self.author, self.location)
    }
}

pub struct Tweet {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}

impl Summary for Tweet {
    fn summarize(&self) -> String {
        format!("{}: {}", self.username, self.content)
    }
}

fn returns_summarizable() -> impl Summary {
    Tweet {
        username: String::from("horse_ebooks"),
        content: String::from(
            "of course, as you probably already know, people",
        ),
        reply: false,
        retweet: false,
    }
}
```

### Использование ограничений типажа для условной реализации методов

Используя ограничение типажа с блоком `impl`, который использует параметры обобщённого типа, можно реализовать методы условно, для тех типов, которые реализуют указанный типаж. Например, тип `Pair<T>` в листинге 10-16 всегда реализует функцию `new`. Но `Pair<T>`реализует метод `cmp_display` только если его внутренний тип `T` реализует типаж `PartialOrd`(позволяющий сравнивать) _и_ типаж `Display` (позволяющий выводить на печать).

Файл: src/lib.rs
```rust
use std::fmt::Display;

struct Pair<T> {
    x: T,
    y: T,
}

impl<T> Pair<T> {
    fn new(x: T, y: T) -> Self {
        Self { x, y }
    }
}

impl<T: Display + PartialOrd> Pair<T> {
    fn cmp_display(&self) {
        if self.x >= self.y {
            println!("The largest member is x = {}", self.x);
        } else {
            println!("The largest member is y = {}", self.y);
        }
    }
}
```
Мы также можем условно реализовать типаж для любого типа, который реализует другой типаж. Реализации типажа для любого типа, который удовлетворяет ограничениям типажа называются _общими реализациями_ и широко используются в стандартной библиотеке Rust. Например, стандартная библиотека реализует типаж `ToString` для любого типа, который реализует типаж `Display`. Блок `impl` в стандартной библиотеке выглядит примерно так:

```rust
impl<T: Display> ToString for T {
    // --snip--
}
```
## Валидация ссылок при помощи времен жизни
	
Основная цель времён жизни состоит в том, чтобы предотвратить недействительные ссылки (dangling references), которые приводят к тому, что программа ссылается на данные отличные от данных на которые она должна ссылаться. Рассмотрим программу из листинга 10-17, которая имеет внешнюю и внутреннюю области видимости.
=======


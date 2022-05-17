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



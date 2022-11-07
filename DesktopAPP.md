# QT desktop app for Unit blockchain

## Инициализация приложения с фразой

1) При открытии приложения следует вывести поле для ввода мнемонической фразы(если фразы нет, см [инициализия без фразы](#инициализация-приложения-без-фразы)). Формы релизации:
   1) Одно главное меню, где из мнемонической фразы получается "вытащить" адрес и соотвественно подгрузить баланс.
   2) Используя базу данных RocksDB хранить мнемоническую фразу в базе данных и при старте приложения совершать проверку на наличие записи в базе данных. ***Требуется*** решить проблему сохранения состояния о пути к файлам базы данных (* более сложная задача). Возможно требуется создание installer'а для Windows и подумать над подобной реализации для linux, к примеру bash скрипт или хранить файл-конфигурацию в одинаковой директории для всех Unix-like платформ(к примеру, директория ***etc***)
   3) При старте приложения выводить только поле ввода мнемонической фразы, после удачного парсинга которой будет получен доступ к следующему "экрану", где будет кнопка перевода, поле с отображаемым балансом, номером аккаунта, адресом. Референс - **Metamask**

## Инициализация приложения без фразы

1. Если фразы нет - предложить создать фразу исходя из примера ниже([полный пример использования подписей: от создания кошелька до валидации подписанного и восстановлении адреса](#полный-пример-использования-подписей:-от-создания-кошелька-до-валидации-подписанного-и-восстановлении-адреса)). 
2. После создания необходимо показать фразу пользователю, чтобы он записал ее куда-то. 
3. Переход в главное меню со всем функционалом

## Получение адреса из мнемонической фразы

В [**каталоге**](https://github.com/Unit-chain/Unit-reborn) проекта на GitHub есть папка, называемаемая **[bip44](https://github.com/Unit-chain/Unit-reborn/tree/main/bip44)**, в ней хранятся все необходимое для кошельков. Чтобы восстановить адрес по мнемонической фразе, требуется провести следующее:
```c++
BIP39Result bip39Result = BIP39Result(mnemonic, passphrase);
BIP44 bip44 = BIP44();
BIP44Result bip44Result = bip44.generateAddress(bip39Result, 0, EXTERNAL_CHANGE, 0);
```

В `BIP44Result` будут содержаться все интересующие нас поля.

## Отправка транзакций

Чтобы отправить/подписать транзакцию, необходимо из библиотеки `libdevcore`, находящейся в [**каталоге**](https://github.com/Unit-chain/Unit-reborn) проекта, необходимо будет использовать класс `RawTransaction.h` и инициализировав, сериализовать транзакцию, используя метод `serializeWithoutSignatures`, после чего полученную строку необходимо подписать. После того, как была подписана транзакция и серилизована, согласно файлу [RPC-запросов](https://github.com/Unit-chain/Unit-reborn/blob/main/RPC_REQUESTS.md), отправить, используя метод `transfer`, сериализованную транзакцию, полученную методом `serializeForSending`. Далее используя возможности библиотеки **Boost** и встроенных http запросов - отправить транзакцию по указанному домену(и/или ip адресу).

### Полный пример использования подписей: от создания кошелька до валидации подписанного и восстановлении адреса

```c++
				BIP44 bip44;
        BIP44Result r = bip44.generateWallet(PHRASE_24, 0, EXTERNAL_CHANGE);
        cout << "WALLET PATH: " << r.path << endl;
        cout << "WALLET PHRASE: " << r.mnemonic.phrase << endl;
        cout << "WALLET SEED: " << r.mnemonic.seed << endl;
        cout << "WALLET PRV: " << r.prv << endl;
        cout << "WALLET PUB: " << r.pub << endl;
        cout << "WALLET E_PRV: " << r.extended_prv << endl;
        cout << "WALLET E_PUB: " << r.extended_pub << endl;
        cout << "WALLET ADDRESS: " << r.address << endl;

        cout << "\nSigning message 'Hello!': \n";
        ECDSASignResult sig = ecdsa_sign_message("Hello!", r.prv);
        std::cout << "r: " << sig.r << std::endl;
        std::cout << "s: " << sig.s << std::endl;
        std::cout << "signature hash: " << sig.message_hash << std::endl;
        bool verified = ecdsa_verify_signature(sig.r, sig.s, "Hello!", r.address);
        cout << "Result of verification: "<<verified<<"\n";
        BIP44Result r2 = bip44.generateAddress(r.mnemonic, 0, EXTERNAL_CHANGE, 1);
        cout << "\n\nWALLET PATH: " << r2.path << endl;
        cout << "WALLET SEED: " << r2.mnemonic.seed << endl;
        cout << "WALLET E_XPRV: " << r2.extended_prv << endl;
        cout << "WALLET E_XPUB: " << r2.extended_pub << endl;
        cout << "WALLET ADDRESS: " << r2.address << endl;

        cout << "\nSigning message 'Hello!': \n";
        ECDSASignResult sig2 = ecdsa_sign_message("Hello!", r2.prv);
        std::cout << "r: " << sig2.r << std::endl;
        std::cout << "s: " << sig2.s << std::endl;
        bool verified2 = ecdsa_verify_signature(sig2.r, sig2.s, "Hello!", r2.address);
        cout << "Result of verification: "<<verified2<<"\n";
```

## Стэк библиотек

* Boost
* OpenSSL
* RocksDB

Без них приложение собираться не будет.

## Опциональные пожелания(необязательны к релизации)

Иметь возможность синхронизировать все данные о кошельке, не вставляя постоянно мнемоническую фразу в приложение. Плюс автоматически инкрементировать `nonce`(опять же вопрос к синхронизации)
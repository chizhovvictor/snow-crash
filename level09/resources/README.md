Level09
=======

Flag
----

s5cAJpM8ev6XHw998pRWG728z

* * *

Discovery
---------

### Проверка директории

    level09@SnowCrash:~$ ls -l
    total 12
    -rwsr-sr-x 1 flag09 level09 7640 Mar  5  2016 level09
    ----r--r-- 1 flag09 level09   26 Mar  5  2016 token

Наблюдение: есть исполняемый файл **level09** с SUID/SGID битами и файл **token**, который возможно содержит закодированный токен.

### Проверка файла level09

    level09@SnowCrash:~$ ./level09 
    You need to provied only one arg.

Программа требует аргумент. Динамический анализ через ltrace:

    level09@SnowCrash:~$ ltrace ./level09 
    __libc_start_main(0x80487ce, 1, 0xbffff7f4, 0x8048aa0, 0x8048b10 ptrace(0, 0, 1, 0, 0xb7e2fe38)          = -1
    puts("You should not reverse this"You should not reverse this)     = 28
    +++ exited (status 1) +++

Проверим с аргументом:

    level09@SnowCrash:~$ ./level09 token 
    tpmhr
    level09@SnowCrash:~$ cat token 
    f4kmm6p|=�p�n��DB�Du{��
    level09@SnowCrash:~$ ./level09 adfgkn
    aehjos
    level09@SnowCrash:~$ ./level09 aaaa

Заметно, что программа выводит строку, которая выглядит как сдвинутая по символам. Каждый следующий символ кажется смещённым сильнее, чем предыдущий. Мое предположение, что это не случайный набор символов, а простая арифметическая операция над ASCII‑кодами. Проверил: первый символ отличается на 1, второй на 2, третий на 3… Возможно токен находится в файле **token** просто нам его нужно декодировать в обратном направлении.

* * *

Use (Exploit)
-------------

### Декодирование токена

Скопируем файл **token** в /tmp/ и сделаем его читаемым:

    level09@SnowCrash:~$ cp token /tmp/
    level09@SnowCrash:~$ chmod +r /tmp/token
    level09@SnowCrash:~$ cat /tmp/token
    f4kmm6p|=�p�n��DB�Du{��

С помощью команды **scp** передаем файл на локальную машину для обработки:

    f4r7s10% scp -P 4242 level09@192.168.56.107:/tmp/token ./level09/resources

Пишем маленькую программу **reverse.c**, которая вычитает позицию из ASCII-кода каждого символа для обратной дешифровки.
Компиляция и запуск:

    f4r7s10% gcc reverse.c
    f4r7s10% ./a.out token
    f3iji1ju5yuevaus41q1afiuq

Это расшифрованный токен.

### Получение флага

    level09@SnowCrash:~$ su flag09
    Password: 
    Don't forget to launch getflag !
    flag09@SnowCrash:~$ getflag
    Check flag.Here is your token : s5cAJpM8ev6XHw998pRWG728z

Флаг успешно получен через декодирование содержимого файла **token**.

* * *

Prevention
----------

*   Не хранить чувствительные данные в легко доступных файлах без защиты.
*   Использовать надежные методы шифрования вместо простых сдвигов ASCII.
*   Ограничить права на SUID/SGID файлы и контролировать доступ к ним.

* * *

Documentation
-------------

*   man ltrace
*   man chmod
*   man su
*   man scp
*   ASCII кодировка символов
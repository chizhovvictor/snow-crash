Level13
==========================

Flag
----

2A31L79asukciNyi8uppkEuSx

* * *

Discovery
---------

### Проверка директории

    level13@SnowCrash:~$ ls -l
    total 8
    -rwsr-sr-x 1 flag13 level13 7303 Aug 30  2015 level13

Запускаем программу:

    level13@SnowCrash:~$ ./level13 
    UID 2013 started us but we we expect 4242

Проверка ltrace:

    level13@SnowCrash:~$ ltrace ./level13 
    __libc_start_main(0x804858c, 1, 0xbffff7f4, 0x80485f0, 0x8048660 
    getuid()                                = 2013
    getuid()                                = 2013
    printf("UID %d started us but we we expe"..., 2013UID 2013 started us but we we expect 4242) = 42
    exit(1 +++ exited (status 1) +++

Анализ:

*   Программа вызывает `getuid()` дважды.
*   Ожидаемый UID — **4242**.
*   Наш UID — **2013** (level13), поэтому программа выходит.

Программа проверяет, что её UID равен определённому значению (в оригинале это 4242). Если UID не совпадает, она отказывается работать. Мы запускаем её как пользователь level13 (UID=2013), поэтому проверка не проходит.

### Проблема

Для обхода проверки нужно подменить результат `getuid()`. Есть два подхода:

*   **LD_PRELOAD** — подменить функцию динамической библиотекой.
*   **GDB** — прервать программу и вернуть другое значение из getuid().

> LD_PRELOAD is an environment variable in Linux that specifies a shared library to be loaded before any other libraries. It is used to override functions in the default libraries with custom implementations. When a program is executed, the dynamic linker (ld.so) searches for the shared libraries required by the program. If LD_PRELOAD is set, the specified library is loaded first, even before the standard libraries like libc. https://medium.com/@hemparekh1596/ld-preload-and-dynamic-library-hijacking-in-linux-237943abb8e0

* * *

Use (Exploit)
-------------

### Попытка LD_PRELOAD

Пишем `fakeuid.c` со своей реализацией `getuid()`, компилируем как .so:

    f4r7s10% gcc -Wall -fPIC -shared -o fakeuid.so fakeuid.c

Переносим на машину:

    f4r7s10% scp -P 4242 ./fakeuid.so level13@192.168.56.107:/tmp/fakeuid.so
    fakeuid.so                       100%   15KB  31.6MB/s   00:00

Пробуем запустить:

    LD_PRELOAD=/tmp/fakeuid.so ./level13

**Но это не сработало.** На старых Ubuntu в виртуалке часто включён безопасный режим загрузчика (ld.so), запрещающий LD_PRELOAD для SUID-бинарников.

* * *

### Рабочий способ — GDB

Раз LD_PRELOAD блокируется, то остаётся поменять значение `getuid()` через отладчик.

    level13@SnowCrash:~$ gdb ./level13
    GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1)
    Reading symbols from /home/user/level13/level13...(no debugging symbols found)...done.

Ставим брейкпоинт на getuid:

    (gdb) break getuid
    Breakpoint 1 at 0x8048380

Запускаем:

    (gdb) run
    Starting program: /home/user/level13/level13 
    Breakpoint 1, 0xb7ee4cc0 in getuid () from /lib/i386-linux-gnu/libc.so.6

Подменяем возвращаемое значение return (Если gdb жалуется на тип, (int) обычно проходит. Можно попробовать (uid_t)4242, если тип доступен.):

    (gdb) return (int)4242
    Make selected stack frame return now? (y or n) y

Продолжаем выполнение:

    #0  0x0804859a in main ()
    (gdb) continue
    Continuing.
    your token is 2A31L79asukciNyi8uppkEuSx
    [Inferior 1 (process 5254) exited with code 050]

Успех — программа приняла «правильный» UID и раскрыла флаг.

```
level13@SnowCrash:~$ gdb ./level13
GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
Copyright (C) 2012 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-linux-gnu".
For bug reporting instructions, please see:
<http://bugs.launchpad.net/gdb-linaro/>...
Reading symbols from /home/user/level13/level13...(no debugging symbols found)...done.
(gdb) break getuid
Breakpoint 1 at 0x8048380
(gdb) run
Starting program: /home/user/level13/level13 

Breakpoint 1, 0xb7ee4cc0 in getuid () from /lib/i386-linux-gnu/libc.so.6
(gdb) return (int)4242
Make selected stack frame return now? (y or n) y
#0  0x0804859a in main ()
(gdb) continue
Continuing.
your token is 2A31L79asukciNyi8uppkEuSx
[Inferior 1 (process 5254) exited with code 050]
(gdb) continue
```

Prevention
----------

*   Не полагаться на `getuid()` как на защиту — это легко подделать.
*   Использовать безопасные механизмы контроля доступа (capabilities, sudoers и т.д.).
*   Отключать возможность запуска SUID-файлов под gdb в продакшене.
*   Отключать отладочную информацию и использовать seccomp/AppArmor.

* * *

Documentation
-------------

*   man 2 getuid
*   man ld.so (раздел про LD\_PRELOAD)
*   gdb cheat sheet
*   Dynamic loader & SUID security
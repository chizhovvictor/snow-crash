0) смотрим какие пользователи есть в системе
<<<
level01@SnowCrash:~$ cat /etc/passwd
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/bin/sh
bin:x:2:2:bin:/bin:/bin/sh
sys:x:3:3:sys:/dev:/bin/sh
sync:x:4:65534:sync:/bin:/bin/sync
games:x:5:60:games:/usr/games:/bin/sh
man:x:6:12:man:/var/cache/man:/bin/sh
lp:x:7:7:lp:/var/spool/lpd:/bin/sh
mail:x:8:8:mail:/var/mail:/bin/sh
news:x:9:9:news:/var/spool/news:/bin/sh
uucp:x:10:10:uucp:/var/spool/uucp:/bin/sh
proxy:x:13:13:proxy:/bin:/bin/sh
www-data:x:33:33:www-data:/var/www:/bin/sh
backup:x:34:34:backup:/var/backups:/bin/sh
list:x:38:38:Mailing List Manager:/var/list:/bin/sh
irc:x:39:39:ircd:/var/run/ircd:/bin/sh
gnats:x:41:41:Gnats Bug-Reporting System (admin):/var/lib/gnats:/bin/sh
nobody:x:65534:65534:nobody:/nonexistent:/bin/sh
libuuid:x:100:101::/var/lib/libuuid:/bin/sh
syslog:x:101:103::/home/syslog:/bin/false
messagebus:x:102:106::/var/run/dbus:/bin/false
whoopsie:x:103:107::/nonexistent:/bin/false
landscape:x:104:110::/var/lib/landscape:/bin/false
sshd:x:105:65534::/var/run/sshd:/usr/sbin/nologin
level00:x:2000:2000::/home/user/level00:/bin/bash
level01:x:2001:2001::/home/user/level01:/bin/bash
level02:x:2002:2002::/home/user/level02:/bin/bash
level03:x:2003:2003::/home/user/level03:/bin/bash
level04:x:2004:2004::/home/user/level04:/bin/bash
level05:x:2005:2005::/home/user/level05:/bin/bash
level06:x:2006:2006::/home/user/level06:/bin/bash
level07:x:2007:2007::/home/user/level07:/bin/bash
level08:x:2008:2008::/home/user/level08:/bin/bash
level09:x:2009:2009::/home/user/level09:/bin/bash
level10:x:2010:2010::/home/user/level10:/bin/bash
level11:x:2011:2011::/home/user/level11:/bin/bash
level12:x:2012:2012::/home/user/level12:/bin/bash
level13:x:2013:2013::/home/user/level13:/bin/bash
level14:x:2014:2014::/home/user/level14:/bin/bash
flag00:x:3000:3000::/home/flag/flag00:/bin/bash
flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
flag02:x:3002:3002::/home/flag/flag02:/bin/bash
flag03:x:3003:3003::/home/flag/flag03:/bin/bash
flag04:x:3004:3004::/home/flag/flag04:/bin/bash
flag05:x:3005:3005::/home/flag/flag05:/bin/bash
flag06:x:3006:3006::/home/flag/flag06:/bin/bash
flag07:x:3007:3007::/home/flag/flag07:/bin/bash
flag08:x:3008:3008::/home/flag/flag08:/bin/bash
flag09:x:3009:3009::/home/flag/flag09:/bin/bash
flag10:x:3010:3010::/home/flag/flag10:/bin/bash
flag11:x:3011:3011::/home/flag/flag11:/bin/bash
flag12:x:3012:3012::/home/flag/flag12:/bin/bash
flag13:x:3013:3013::/home/flag/flag13:/bin/bash
flag14:x:3014:3014::/home/flag/flag14:/bin/bash
>>>
1) как вариант можно опять чекнуть что принадлежит пользователям flag01 и level01
<<<
find / -user "flag01" 2>/dev/null ничего не дал 
find / -user "level01" 2>/dev/null только папка proc но ничего особенного в них нет
>>>
2) Можно посмотреть какие файлы пользователь может читать
<<<
find / -type f -perm -o=r 2>/dev/null получается очень много, в них что то найти будет очень сложно
>>>
3) можно посмотреть просто файл с  паролями
<<<
level01@SnowCrash:~$ cat /etc/shadow
cat: /etc/shadow: Permission denied
>>>
4) можно посмотреть все файлы с правами 777
<<<
level01@SnowCrash:~$ find / -type f -perm 777 2>/dev/null
level01@SnowCrash:~$ 
>>>
5) анализируем таблицу паролей
flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
flag01 - user
42hDRfypTqqnw - password (x - password in /etc/shadow)
3001 - user id
3001 - group id
/home/flag/flag01 - home catalog
/bin/bash - default shell
6) попробуем декодировать его
md5 - нет
Caesar - нет
rot13 - нет
7) тогда анализируем данный пароль
===========================
В старых системах пароли действительно хранились прямо в /etc/passwd в виде хэшей.
В современных Linux пароли вынесены в файл /etc/shadow, а в /etc/passwd вместо хэша стоит x.
Строка 42hDRfypTqqnw — это результат работы алгоритма хэширования (например, DES, MD5, SHA‑512).
Настоящий пароль напрямую из этой строки узнать нельзя. Его можно только проверить при входе или пытаться подобрать, сравнивая хэши.
===========================
Как определить алгоритм по виду хэша
SHA‑512: обычно очень длинный хэш (около 86 символов), и в /etc/shadow он начинается с $6$....
MD5: короче, около 32 символов, начинается с $1$....
SHA‑256: около 43 символов, начинается с $5$....
DES (старый алгоритм): всего 13 символов, без префикса $.
===========================
42hDRfypTqqnw → это старый DES‑хэш пароля
Для учебных целей и аудита безопасности в Linux чаще всего используют программы John the Ripper и Hashcat. Они позволяют работать с хэшами паролей (например, DES, MD5, SHA‑512) и проверять их стойкость с помощью словарей или перебора
Пробую Hashcat
<<<
f4r7s10% ./hashcat.bin 42hDRfypTqqnw
hashcat (v7.1.2) starting in autodetect mode

clGetPlatformIDs(): CL_PLATFORM_NOT_FOUND_KHR

ATTENTION! No OpenCL, HIP or CUDA compatible platform found.

You are probably missing the OpenCL, CUDA or HIP runtime installation.

* AMD GPUs on Linux require this driver:
  "AMD Radeon Software for Linux" with "ROCm"
* Intel and AMD CPUs require this runtime:
  "Intel CPU Runtime for OpenCL" or PoCL
* Intel GPUs require this driver:
  "Intel Graphics Compute Runtime" aka NEO
* NVIDIA GPUs require this runtime and driver:
  "NVIDIA CUDA Toolkit" (both runtime and driver included)

Started: Sat Nov 22 14:58:15 2025
Stopped: Sat Nov 22 14:58:15 2025
>>>
===========================
Устанавливаем john the ripper
<<<
git clone https://github.com/openwall/john.git
cd john/src
./configure && make -s clean && make -sj4

или взять в папке john/src выполнить make generic
>>>
===========================
8) Используем john the ripper для декриптинга данного пароля 
<<<
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
>>>
Формат вывода для john username:password, т.е.
? - username
abcdefg - password
9) <<<
flag01@SnowCrash:~$ getflag
Check flag.Here is your token : f2av5il02puano7naaf6adaaf
>>>
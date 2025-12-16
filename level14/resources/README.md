# Level14

## Flag

7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ

## Discovery

### Checking the directory

1) we are looking for files related to level14

```
level14@SnowCrash:~$ find / -user flag14 2>/dev/null
level14@SnowCrash:~$ find / -type f -perm -4000 2>/dev/null | grep 14
level14@SnowCrash:~$ find / -name "level14*" 2>/dev/null
level14@SnowCrash:~$ ls -la
total 12
dr-x------ 1 level14 level14  100 Mar  5  2016 .
d--x--x--x 1 root    users    340 Aug 30  2015 ..
-r-x------ 1 level14 level14  220 Apr  3  2012 .bash_logout
-r-x------ 1 level14 level14 3518 Aug 30  2015 .bashrc
-r-x------ 1 level14 level14  675 Apr  3  2012 .profile
level14@SnowCrash:~$ ls -l /bin/level14
ls: cannot access /bin/level14: No such file or directory
```

We don't find anything

2) check getflag

```
level14@SnowCrash:~$ getflag
Check flag.Here is your token : 
Nope there is no token here for you sorry. Try again :)
```

nothing

3) we check the processes

```
evel14@SnowCrash:~$ ps aux | grep flag14
level14   2233  0.0  0.0   4380   820 pts/0    S+   14:15   0:00 grep --color=auto flag14
level14@SnowCrash:~$ netstat -tulpen
(No info could be read for "-p": geteuid()=2014 but you should be root.)
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       User       Inode       PID/Program name
tcp        0      0 0.0.0.0:4242            0.0.0.0:*               LISTEN      0          12234       -               
tcp        0      0 127.0.0.1:5151          0.0.0.0:*               LISTEN      3011       12345       -               
tcp6       0      0 :::4646                 :::*                    LISTEN      0          10579       -               
tcp6       0      0 :::4747                 :::*                    LISTEN      0          10583       -               
tcp6       0      0 :::80                   :::*                    LISTEN      0          10574       -               
tcp6       0      0 :::4242                 :::*                    LISTEN      0          12237       -               
udp        0      0 0.0.0.0:68              0.0.0.0:*                           0          11897       -               
level14@SnowCrash:~$ curl http://127.0.0.1:5151/
Password: Erf nope..
```
4) check **cat /etc/passwd**
```
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
```
5) we are looking for the getflag program and download it

```
scp -P 4242 level14@<vm_ip>:/bin/getflag ~/Downloads/
```
### Solution

6) decompile the program code on resources like https://dogbolt.org/?id=7497b998-c523-470d-9b4d-2d80c56eee4c#Ghidra=499&Hex-Rays=132&BinaryNinja=194&angr=63

7) get the code and copy it to **main.c**

8) analyzing the code, we see that there is a ft_des function that decompiles the shortened strings.
the ft_des function is called **_Var6 = getuid();**
therefore _Var6 should be equal to our flag14:x:3014:3014

we look at the code and see that there is a hex comparison. converting **3014** to hex, we get **0xbc6**
. our token is the string **g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|**
```
pcVar4 = (char *)ft_des("g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|");
```
9) we take our decompilation function, recreate main and execute. getting a token





---

## rus translate
```
1) ищем файлы сязанные с level14
level14@SnowCrash:~$ find / -user flag14 2>/dev/null
level14@SnowCrash:~$ find / -type f -perm -4000 2>/dev/null | grep 14
level14@SnowCrash:~$ find / -name "level14*" 2>/dev/null
level14@SnowCrash:~$ ls -la
total 12
dr-x------ 1 level14 level14  100 Mar  5  2016 .
d--x--x--x 1 root    users    340 Aug 30  2015 ..
-r-x------ 1 level14 level14  220 Apr  3  2012 .bash_logout
-r-x------ 1 level14 level14 3518 Aug 30  2015 .bashrc
-r-x------ 1 level14 level14  675 Apr  3  2012 .profile
level14@SnowCrash:~$ ls -l /bin/level14
ls: cannot access /bin/level14: No such file or directory
```
ничего не находим

2) проверяем getflag
```
level14@SnowCrash:~$ getflag
Check flag.Here is your token : 
Nope there is no token here for you sorry. Try again :)
```
ничего

3) проверяем процессы
```
evel14@SnowCrash:~$ ps aux | grep flag14
level14   2233  0.0  0.0   4380   820 pts/0    S+   14:15   0:00 grep --color=auto flag14
level14@SnowCrash:~$ netstat -tulpen
(No info could be read for "-p": geteuid()=2014 but you should be root.)
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       User       Inode       PID/Program name
tcp        0      0 0.0.0.0:4242            0.0.0.0:*               LISTEN      0          12234       -               
tcp        0      0 127.0.0.1:5151          0.0.0.0:*               LISTEN      3011       12345       -               
tcp6       0      0 :::4646                 :::*                    LISTEN      0          10579       -               
tcp6       0      0 :::4747                 :::*                    LISTEN      0          10583       -               
tcp6       0      0 :::80                   :::*                    LISTEN      0          10574       -               
tcp6       0      0 :::4242                 :::*                    LISTEN      0          12237       -               
udp        0      0 0.0.0.0:68              0.0.0.0:*                           0          11897       -               
level14@SnowCrash:~$ curl http://127.0.0.1:5151/
Password: Erf nope..
```
4) проверяем cat /etc/passwd
```
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
```
5) ищем программу getflag и скачиваем ее
```
scp -P 4242 level14@<vm_ip>:/bin/getflag ~/Downloads/
```
6) декомпилируем код программы на ресурсах типа https://dogbolt.org/?id=7497b998-c523-470d-9b4d-2d80c56eee4c#Ghidra=499&Hex-Rays=132&BinaryNinja=194&angr=63

7) получаем код и копируем его в **main.c**

8) анализируем код, видим что есть функция ft_des, которая декомплирует захардкоженные строки.
функция ft_des вызывается **_Var6 = getuid();**
следовательно _Var6 должен быть равен нашему flag14:x:3014:3014

смотрим по коду и видим что идет сравнение hex. переводим 3014 в hex, получаем 0xbc6
наш токен - это строка **g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|**
```
pcVar4 = (char *)ft_des("g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|");
```
9) берем нашу функцию декомпиляции, воссоздаем main и выполняем. получаем токен


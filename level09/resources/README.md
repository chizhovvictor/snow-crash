1) Проверяем дерикторию
<<<
level09@SnowCrash:~$ ls -l
total 12
-rwsr-sr-x 1 flag09 level09 7640 Mar  5  2016 level09
----r--r-- 1 flag09 level09   26 Mar  5  2016 token
>>>
2) Проверим файл level09
<<<
level09@SnowCrash:~$ ./level09 
You need to provied only one arg.

level09@SnowCrash:~$ ltrace ./level09 
__libc_start_main(0x80487ce, 1, 0xbffff7f4, 0x8048aa0, 0x8048b10 <unfinished ...>
ptrace(0, 0, 1, 0, 0xb7e2fe38)          = -1
puts("You should not reverse this"You should not reverse this
)     = 28
+++ exited (status 1) +++
>>>

По всей видимости нужно прокинуть аркумент с чем то
<<<
level09@SnowCrash:~$ ./level09 token 
tpmhr
level09@SnowCrash:~$ cat token 
f4kmm6p|=�p�n��DB�Du{��
>>>

Проверим ltrace при выполнении программы с файлом
<<<
level09@SnowCrash:~$ ltrace ./level09 token 
__libc_start_main(0x80487ce, 2, 0xbffff7d4, 0x8048aa0, 0x8048b10 <unfinished ...>
ptrace(0, 0, 1, 0, 0xb7e2fe38)          = -1
puts("You should not reverse this"You should not reverse this
)     = 28
+++ exited (status 1) +++
>>>

Попробуем просто прокинуть строку
<<<
level09@SnowCrash:~$ ./level09 adfgkn
aehjos
level09@SnowCrash:~$ ./level09 aaaa
abcd
>>>

Заметно, что программа выводит строку, которая выглядит как сдвинутая по символам. Каждый следующий символ кажется смещённым сильнее, чем предыдущий. Мое предположение, что это не случайный набор символов, а простая арифметическая операция над ASCII‑кодами. Проверил: первый символ отличается на 1, второй на 2, третий на 3… Возможно токен находится в файле токен просто нам его нужно декодировать в обратном направлении.
3) Напишем маленький скрипт, который вычитает позицию из ASCII‑кода каждого символа. И скопируем при помощи команды scp файл token
<<<
level09@SnowCrash:~$ cp token /tmp/
level09@SnowCrash:~$ cat /tmp/token
cat: /tmp/token: Permission denied
level09@SnowCrash:~$ chmod +r /tmp/token
level09@SnowCrash:~$ cat /tmp/token
f4kmm6p|=�p�n��DB�Du{��
level09@SnowCrash:~$ 
>>>

Выполним с хоста
<<<
f4r7s10% scp -P 4242 level09@192.168.56.107:/tmp/token ./level09/resources
>>>

Теперь скомпелируем скрипт reverse.c и прогоним наш файл token через него
<<<
f4r7s10% gcc reverse.c
f4r7s10% ./a.out token
f3iji1ju5yuevaus41q1afiuq
>>>

<<<
level09@SnowCrash:~$ su flag09
Password: 
Don't forget to launch getflag !
flag09@SnowCrash:~$ getflag
Check flag.Here is your token : s5cAJpM8ev6XHw998pRWG728z
>>>
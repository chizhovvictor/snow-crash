1) в начале ищем файлы относящиеся к пользователю, у нас их 2 - флаг и левел. ищем по флагу, находим 2 файла. ищем по левелу находим убитый процесс
2) смотрим по флагу

level00@SnowCrash:~$ find / -user "flag00" 2>/dev/null
/usr/sbin/john
/rofs/usr/sbin/john

перенаправляем ошибки 2>/dev/null в нул чтобы очистить терминал от ошибок и получить только выполненые результаты

3) смотрим с помощью cat что за файлы
level00@SnowCrash:~$ cat /usr/sbin/john
cdiiddwpgswtgt
level00@SnowCrash:~$ cat /rofs/usr/sbin/john
cdiiddwpgswtgt

4) данный код не подходит для пароля юезра флаг00
5) Текущий результат — строка cdiiddwpgswtgt — может быть закодированной или зашифрованной информацией. Нужно использовать методы дешифровки или декодирования, а также проверить ее с помощью программы John the Ripper поскольку название файла скорее всего ведет к названия ютилиты.

6) есть несколько методов декодирования
Caesar Cipher
Base64 Encoding
ROT13 Cipher
XOR-encoded
Anagram or Simple Substitution Cipher

7) на форумах (гитхабе) нашел сайт dcode.fr
8) провел анализ возможных шифров
выдало 9:
Chiffre Ragbaby	
Chiffre Affine	
Substitution Mono-alphabétique	
Disque Chiffrant	
Codage Base62	
Base 58	
Chiffre ROT (Rotation)	
Code César	
Code Morse Fractionné

методом перебора (гитхаба) нашел César и использовал первый фариант, ключ готов

nottoohardhere


Conclusion: Which Decode Method to Use?
If it looks like shifted letters → Caesar, ROT13, Atbash

If it's jumbled letters → Transposition ciphers

If it contains =, /, or numbers → Base64, Hex, or Binary

If it's unreadable but similar length to input → XOR

If it’s a long string of letters/numbers → Hashing (MD5, SHA)

If there are hidden messages in images or files → Steganography




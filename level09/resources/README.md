# Level09

## Flag

s5cAJpM8ev6XHw998pRWG728z

---

## Discovery

### Checking the directory

```
level09@SnowCrash:~$ ls -l
total 12
-rwsr-sr-x 1 flag09 level09 7640 Mar  5  2016 level09
----r--r-- 1 flag09 level09   26 Mar  5  2016 token
```

Observation: there is an executable file **level09** with SUID/SGID bits and a **token** file, which may contain an encoded token.

### Checking the level09 file

```
level09@SnowCrash:~$ ./level09 
You need to provied only one arg.
```

The program requires an argument. Dynamic analysis using ltrace:

```
level09@SnowCrash:~$ ltrace ./level09 
__libc_start_main(0x80487ce, 1, 0xbffff7f4, 0x8048aa0, 0x8048b10 ptrace(0, 0, 1, 0, 0xb7e2fe38)          = -1
puts("You should not reverse this"You should not reverse this)     = 28
+++ exited (status 1) +++
```

Let's test it with an argument:

```
level09@SnowCrash:~$ ./level09 token 
tpmhr
level09@SnowCrash:~$ cat token 
f4kmm6p|=�p�n��DB�Du{��
level09@SnowCrash:~$ ./level09 adfgkn
aehjos
level09@SnowCrash:~$ ./level09 aaaa
```

It is noticeable that the program outputs a string that looks like it’s shifted by characters. Each next character appears to be shifted more than the previous one. My assumption is that this is not a random sequence of characters but a simple arithmetic operation on ASCII codes. Checked: the first character differs by 1, the second by 2, the third by 3... It is possible that the token is stored in the **token** file, we just need to decode it in reverse.

---

## Use (Exploit)

### Decoding the token

Copy the **token** file to /tmp/ and make it readable:

```
level09@SnowCrash:~$ cp token /tmp/
level09@SnowCrash:~$ chmod +r /tmp/token
level09@SnowCrash:~$ cat /tmp/token
f4kmm6p|=�p�n��DB�Du{��
```

Using **scp**, transfer the file to the local machine for processing:

```
f4r7s10% scp -P 4242 level09@192.168.56.107:/tmp/token ./level09/resources
```

Write a small program **reverse.c** that subtracts the character's position from the ASCII code for reverse decoding.
Compile and run:

```
f4r7s10% gcc reverse.c
f4r7s10% ./a.out token
f3iji1ju5yuevaus41q1afiuq
```

This is the decoded token.

### Getting the flag

```
level09@SnowCrash:~$ su flag09
Password: 
Don't forget to launch getflag !
flag09@SnowCrash:~$ getflag
Check flag.Here is your token : s5cAJpM8ev6XHw998pRWG728z
```

The flag was successfully retrieved by decoding the contents of the **token** file.

---

## Prevention

* Do not store sensitive data in easily accessible files without protection.
* Use secure encryption methods instead of simple ASCII shifts.
* Restrict permissions on SUID/SGID files and control access to them.

---

## Documentation

* man ltrace
* man chmod
* man su
* man scp
* ASCII character encoding
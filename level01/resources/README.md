# Level01

## Flag

f2av5il02puano7naaf6adaaf

## Discovery

Started with basic system analysis. First, I decided to check which users existed at all:

```
cat /etc/passwd
```

The output was long, but I was interested in two groups of users: **levelXX** and **flagXX**. It’s visible that the users have predictable UIDs: level01 — 2001, flag01 — 3001.

I tried checking which files belong to these users:

```
find / -user "flag01" 2>/dev/null
find / -user "level01" 2>/dev/null
```

There was almost no result. Then I tried searching for files by permissions:

```
find / -type f -perm -o=r 2>/dev/null
```

Too many files — dead end.

Tried to read the shadow file:

```
cat /etc/shadow
```

As expected: *Permission denied*.

Also checked for world-writable files:

```
find / -type f -perm 777 2>/dev/null
```

Empty. After this, I went back to `/etc/passwd` and noticed that the `flag01` user’s password hash is located right there:

```
flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
```

This is extremely unusual for modern systems and indicates that I should try to crack the hash. I analyzed it:

* not MD5 — format doesn’t match;
* not SHA‑512 — too short;
* not SHA‑256;
* not ROT13, not Caesar;

13 characters → this is **old DES crypt(3)**.

Tried hashcat, but it didn’t run without a GPU backend:

```
./hashcat.bin 42hDRfypTqqnw
```

Switched to John the Ripper.

## Use (Exploit)

Installed John the Ripper:

```
git clone https://github.com/openwall/john.git
cd john/src
./configure && make -s clean && make -sj4
(or go into john/src and run make generic)
```

Created the file `hashes.txt`:

```
flag01:42hDRfypTqqnw
```

Ran John:

```
./john ./hashes.txt
```

It detected the format and cracked the password almost immediately:

abcdefg

Verified via `--show`:

```
?:abcdefg
```

```
f4r7s10% pwd
/home/tgildero/Desktop/snow-crash
f4r7s10% ./level01/resources/john/run/john ./level01/resources/hashes.txt
Using default input encoding: UTF-8
Loaded 1 password hash (descrypt, traditional crypt(3) [DES 256/256 AVX2])
Will run 20 OpenMP threads
Note: Passwords longer than 8 truncated (property of the hash)
Proceeding with single, rules:Single
Press 'q' or Ctrl-C to abort, 'h' for help, almost any other key for status
Almost done: Processing the remaining buffered candidate passwords, if any.
Proceeding with wordlist:./level01/resources/john/run/password.lst
Enabling duplicate candidate password suppressor using 256 MiB
abcdefg          (?)     
1g 0:00:00:00 DONE 2/3 (2025-11-22 15:24) 6.250g/s 768000p/s 768000c/s 768000C/s 123456..140802
Use the "--show" option to display all of the cracked passwords reliably
Session completed
f4r7s10% ./level01/resources/john/run/john ./level01/resources/hashes.txt --show
?:abcdefg

1 password hash cracked, 0 left
```

Now I can log in as the user:

```
su flag01
getflag
```

Result:

Check flag. Here is your token : f2av5il02puano7naaf6adaaf

## Prevention

Main mistakes of this level:

### Password hash stored in `/etc/passwd`

This allows any user to read the hash and try to crack it. In older systems, passwords were indeed stored directly in /etc/passwd in hashed form. In modern Linux, passwords are stored in /etc/shadow, and in /etc/passwd the hash is replaced with x.
The string 42hDRfypTqqnw is the result of a hashing algorithm (e.g., DES, MD5, SHA‑512).
The real password cannot be directly extracted from this string. It can only be verified during login or cracked by comparing hashes.

For educational purposes and security auditing in Linux, tools like John the Ripper and Hashcat are used. They allow working with password hashes (DES, MD5, SHA‑512, etc.) and evaluating their strength via wordlists or brute-force.

### Use of outdated DES crypt(3)

DES is limited to 8 characters and is easy to brute‑force.

Correct approach:

* store hashes in `/etc/shadow`;
* allow access only to root;
* use **SHA‑512** (`$6$`);
* enable password complexity policy (pam_pwquality).

## Documentation

* `man passwd`
* `man crypt`
* `man shadow`
* John the Ripper — [https://www.openwall.com/john](https://www.openwall.com/john)
* crypt(3) — [https://man7.org/linux/man-pages/man3/crypt.3.html](https://man7.org/linux/man-pages/man3/crypt.3.html)

## Automatic Testing

Minimal security test:

```
#!/bin/bash

grep -q "flag01" /etc/passwd || exit 1

HASH=$(grep flag01 /etc/passwd | cut -d: -f2)

if [[ ${#HASH} -eq 13 ]]; then
  echo "Warning: DES hash detected"
  exit 1
fi

echo "OK: No weak hashes detected"
```

---
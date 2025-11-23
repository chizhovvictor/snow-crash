# Level01

## Flag

f2av5il02puano7naaf6adaaf

## Discovery

I began with basic system analysis. First, I checked which users exist:

```
cat /etc/passwd
```

The output was long, but I was interested in two groups of users: **levelXX** and **flagXX**. Their UIDs follow a predictable pattern: level01 — 2001, flag01 — 3001.

Then I tried to locate files owned by these users:

```
find / -user "flag01" 2>/dev/null
find / -user "level01" 2>/dev/null
```

Almost no results. Next, I attempted to search by permissions:

```
find / -type f -perm -o=r 2>/dev/null
```

Too many files — a dead end.

I tried reading `/etc/shadow`:

```
cat /etc/shadow
```

As expected: *Permission denied*.

I also checked for world‑writable files:

```
find / -type f -perm 777 2>/dev/null
```

Nothing. After that, I returned to `/etc/passwd` and noticed that the password hash for user `flag01` is stored right there:

```
flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash

flag01 - user
42hDRfypTqqnw - password (x - password in /etc/shadow)
3001 - user id
3001 - group id
/home/flag/flag01 - home catalog
/bin/bash - default shell
```

This is extremely unusual for modern systems and indicates that we should try cracking the hash. I analyzed it:

* not MD5 — format doesn't match
* not SHA‑512 — too short
* not SHA‑256
* not ROT13 / not Caesar
* 13 characters → **old DES crypt(3)**

I tried hashcat, but it wouldn’t run without a GPU backend:

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
```

(or simply run `make generic` inside `john/src`)

Created `hashes.txt`:

```
42hDRfypTqqnw
```

Ran John:

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

John detected the hash type and cracked the password almost immediately:

```
abcdefg
```

Verified via `--show`:

```
?:abcdefg
```

Full sample session:

```
Loaded 1 password hash (descrypt, traditional crypt(3))
abcdefg          (?)     
Session completed
```

Now I could log in:

```
su flag01
getflag
```

Output:

```
Check flag. Here is your token : f2av5il02puano7naaf6adaaf
```

## Prevention

Main security issues of this level:

### The password hash is stored inside `/etc/passwd`

This allows any user to read the hash and attempt offline cracking. In old UNIX systems this was normal, but modern Linux stores password hashes in `/etc/shadow`, while `/etc/passwd` contains only an `x`.

### Use of outdated DES crypt(3)

DES truncates passwords to 8 characters and is trivial to brute‑force.

Correct security measures:

* store hashes in `/etc/shadow`;
* restrict access to shadow (root‑only);
* use **SHA‑512** (`$6$`) hashing;
* enforce password complexity policies (`pam_pwquality`);
* regularly audit the system for weak or legacy hashes.

## Documentation

* `man passwd`
* `man crypt`
* `man shadow`
* John the Ripper — [https://www.openwall.com/john](https://www.openwall.com/john)
* crypt(3) — [https://man7.org/linux/man-pages/man3/crypt.3.html](https://man7.org/linux/man-pages/man3/crypt.3.html)

## Automatic Testing

Minimal security test:

```bash
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

Если хочешь, могу так же оформить **Level02** или перевести и структурировать остальные уровни.

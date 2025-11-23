# Level13

## Flag

2A31L79asukciNyi8uppkEuSx

---

## Discovery

### Checking the directory

```
level13@SnowCrash:~$ ls -l
total 8
-rwsr-sr-x 1 flag13 level13 7303 Aug 30  2015 level13
```

Run the program:

```
level13@SnowCrash:~$ ./level13 
UID 2013 started us but we we expect 4242
```

Check with ltrace:

```
level13@SnowCrash:~$ ltrace ./level13 
__libc_start_main(0x804858c, 1, 0xbffff7f4, 0x80485f0, 0x8048660 
getuid()                                = 2013
getuid()                                = 2013
printf("UID %d started us but we we expe"..., 2013UID 2013 started us but we we expect 4242) = 42
exit(1 +++ exited (status 1) +++
```

Analysis:

* The program calls `getuid()` twice.
* Expected UID is **4242**.
* Our UID is **2013** (level13), so the program exits.

The program checks that its UID matches a specific value (4242 in the original). If it doesn’t match, it refuses to run. Since we run it as level13 (UID=2013), the check fails.

### The issue

To bypass the check, we need to spoof the result of `getuid()`. Two possible approaches:

* **LD_PRELOAD** — override the function using a shared library.
* **GDB** — interrupt the program and return a different value from getuid().

> LD_PRELOAD is an environment variable in Linux that specifies a shared library to be loaded before any other libraries. It is used to override functions in the default libraries with custom implementations. When a program is executed, the dynamic linker (ld.so) searches for the shared libraries required by the program. If LD_PRELOAD is set, the specified library is loaded first, even before the standard libraries like libc. [https://medium.com/@hemparekh1596/ld-preload-and-dynamic-library-hijacking-in-linux-237943abb8e0](https://medium.com/@hemparekh1596/ld-preload-and-dynamic-library-hijacking-in-linux-237943abb8e0)

---

## Use (Exploit)

### Attempting LD_PRELOAD

Write `fakeuid.c` with our own implementation of `getuid()`, compile it as .so:

```
f4r7s10% gcc -Wall -fPIC -shared -o fakeuid.so fakeuid.c
```

Transfer to the machine:

```
f4r7s10% scp -P 4242 ./fakeuid.so level13@192.168.56.107:/tmp/fakeuid.so
fakeuid.so                       100%   15KB  31.6MB/s   00:00
```

Try running:

```
LD_PRELOAD=/tmp/fakeuid.so ./level13
```

**But this didn’t work.**
On older Ubuntu versions in the VM, the loader (ld.so) often enables a secure mode that blocks LD_PRELOAD for SUID binaries.

---

### Working method — GDB

Since LD_PRELOAD is blocked, the remaining option is to change the value returned by `getuid()` through the debugger.

```
level13@SnowCrash:~$ gdb ./level13
GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1)
Reading symbols from /home/user/level13/level13...(no debugging symbols found)...done.
```

Set a breakpoint at getuid:

```
(gdb) break getuid
Breakpoint 1 at 0x8048380
```

Run:

```
(gdb) run
Starting program: /home/user/level13/level13 
Breakpoint 1, 0xb7ee4cc0 in getuid () from /lib/i386-linux-gnu/libc.so.6
```

Override the return value (if gdb complains about type, (int) usually works; you can try (uid_t)4242 if available):

```
(gdb) return (int)4242
Make selected stack frame return now? (y or n) y
```

Continue execution:

```
#0  0x0804859a in main ()
(gdb) continue
Continuing.
your token is 2A31L79asukciNyi8uppkEuSx
[Inferior 1 (process 5254) exited with code 050]
```

Success — the program accepted the “correct” UID and revealed the flag.

Full session:

```
level13@SnowCrash:~$ gdb ./level13
GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
Copyright (C) 2012 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-linux-gnu".
For bug reporting instructions, please see:
<http://bugs.launchpad.net/gdb-linaro/>...
Reading symbols from /home/user/level13/level13...(no debugging symbols found)...done.
(gdb) break getuid
Breakpoint 1 at 0x8048380
(gdb) run
Starting program: /home/user/level13/level13 

Breakpoint 1, 0xb7ee4cc0 in getuid () from /lib/i386-linux-gnu/libc.so.6
(gdb) return (int)4242
Make selected stack frame return now? (y or n) y
#0  0x0804859a in main ()
(gdb) continue
Continuing.
your token is 2A31L79asukciNyi8uppkEuSx
[Inferior 1 (process 5254) exited with code 050]
(gdb) continue
```

---

## Prevention

* Don’t rely on `getuid()` for security — it is easy to spoof.
* Use proper access control mechanisms (capabilities, sudoers, etc.).
* Disable the ability to run SUID binaries under gdb in production.
* Strip debugging symbols and use seccomp/AppArmor.

---

## Documentation

* man 2 getuid
* man ld.so (section about LD_PRELOAD)
* gdb cheat sheet
* Dynamic loader & SUID security
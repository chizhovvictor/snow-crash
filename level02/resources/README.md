Файл .pcap (Packet Capture) – это дамп сетевого трафика, который можно анализировать с помощью инструментов вроде Wireshark или tcpdump.

качаем файл на компьютер чтобы прочитать его с помощью wireshark. утилита для чтения сетевого трафика
там 95 пакетов
запускать на своем компьютере
scp -P 4242 level02@<vm_ip>:/home/user/level02/level02.pcap ~/Downloads/

после скачки даем права
chmod 777 level02.pcap

в 43 пакете видим слово пароль в Data, открываем чтобы почитать, follow - tcp stream. переключаем на utf-8, получаем пароль

ft_waNDReL0L


--------------------------------

A .pcap (Packet Capture) file is a dump of network traffic.
It can be analyzed using tools such as Wireshark or tcpdump.

1) Download file on our computer
scp -P 4242 level02@<vm_ip>:/home/user/level02/level02.pcap ~/Downloads/

2) After downloading, give the file full read permissions so Wireshark can open it:
chmod 777 ~/Downloads/level02.pcap

3) Open the file in Wireshark
    Launch Wireshark on your computer.
    Open the file level02.pcap.
    You will see 95 packets captured.

4) Locate the password
    Navigate to packet #43.
    In the Data section, you will see the word “password”.
    Right‑click and choose Follow → TCP Stream.
    Switch the encoding to UTF‑8.
    The decoded stream reveals the password: ft_waNDReL0L
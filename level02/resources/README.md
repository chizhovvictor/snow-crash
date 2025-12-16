# Level02

## Flag

kooda2puivaav1idi4f57q8iq

## Discovery

A .pcap (Packet Capture) file is a dump of network traffic.
It can be analyzed using tools such as Wireshark or tcpdump.

Download file on our computer

```bash
scp -P 4242 level02@<vm_ip>:/home/user/level02/level02.pcap ~/Downloads/
```
After downloading, give the file full read permissions so Wireshark can open it:

```bash
chmod 777 ~/Downloads/level02.pcap
```
* Open the file in Wireshark.
* Launch Wireshark on your computer.
* Open the file level02.pcap.
* You will see 95 packets captured.

**Locate the password**

* Navigate to packet #43.
* In the Data section, you will see the word “password”.
* Right‑click and choose Follow → TCP Stream.
* Switch the encoding to UTF‑8.
* The decoded stream reveals the password: **ft_waNDReL0L**
# TP-LINK ROUTERS

## Wierd configuration
*All the output used here comes from this [post](https://fazlerabbi37.github.io/blogs/mysterious_tp_link_ssh_server.html)*

If you ever scanned your router, you may have seen that ssh is open on the router and even the ssh server version detected.

```nmap
Nmap scan report for _gateway (192.168.0.1)
Host is up (0.0062s latency).
Not shown: 996 closed ports
PORT      STATE SERVICE VERSION
22/tcp    open  ssh     Dropbear sshd 2012.55 (protocol 2.0)
| ssh-hostkey:
|   1024 d2:b1:fa:6c:de:58:d9:17:f7:e5:dd:ba:44:37:39:d4 (DSA)
|_  1040 f8:cc:5b:03:94:db:0c:3f:04:56:d4:76:29:51:5b:0d (RSA)
80/tcp    open  http    TP-LINK WR841N WAP http config
|_http-title: TL-WR841N
1900/tcp  open  upnp    ipOS upnpd (TP-LINK TL-WR841N WAP 11.0; UPnP 1.0)
49152/tcp open  http    Huawei HG8245T modem http config
|_http-title: Site doesn't have a title.
MAC Address: 84:16:F9:FC:9B:CC (Tp-link Technologies)
Service Info: OSs: Linux, ipOS 7.0; Devices: WAP, broadband router; CPE: cpe:/o:linux:linux_kernel, cpe:/h:tp-link:wr841n, cpe:/h:tp-link:tl-wr841n, cpe:/o:ubicom:ipos:7.0, cpe:/h:huawei:hg8245t

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
# Nmap done at Tue Mar  3 13:57:03 2020 -- 1 IP address (1 host up) scanned in 154.57 seconds
```

The interesting thing here is that even knowing the web credentials, we cannot allocate a shell of any kind or execute commands(yes, the username and password are the same across ssh, telnet, and web interface. I am not sure about FTP, though)

```cli
bobcat@what-the-ssh:~/fr37/temp$ ssh admin@192.168.10.1
admin@192.168.10.1's password:
PTY allocation request failed on channel 0
shell request failed on channel 0
bobcat@what-the-ssh:~/fr37/temp$
```

However, we can use this ssh for port forwarding (-L, -R, and -D are supported)

```bash
user@wa1tf0rme$ ssh -D 9050 admin@router.ip
```

And after that, we can use `proxychains` to access webpage like this:

```bash
user@wa1tf0rme$ proxychains curl localhost
```

But, even though we can forward our traffic through the router (remember, we are already in the same local network with the router itself) real point of interest is open ports that are not accessible otherwise as from the localhost. And guess what is accessible? Right, the listener on port `20002` and the coincidence that this particular port is used by the Tether app to execute commands on the router. Hope soon somebody will reverse this communication and provide the reliable tooling to exploit it; for now, we are limited to some CVEs and port forwarding.

## Additional thoughts
If you are interested in the details of how the TP-Link communicates using the Tether app, check this repo on GitHub [tmpcli](https://github.com/ropbear/tmpcli)

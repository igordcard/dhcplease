dhcplease
=========

dhcplease, or DHCP Lease Parser, is a command line application for GNU/Linux that parses a DHCP lease file right away. It is meant to be simple and straight to the point.

After no success in finding an existing application to parse one of my Arch Linux's dhcpcd lease files (/var/lib/dhcpcd/), forcing me to re-request a DHCP lease to capture and analyse the packets, I decided to do it myself. This is the result.

Get it
--------
### Source
You can clone this repository and then compile the program.

<code>git clone git@github.com:igordcard/dhcplease.git</code>

### Arch Linux (AUR)
If you use Arch Linux, or a variant, you can also download this utility through AUR:

[<code>dhcplease</code>](https://aur.archlinux.org/packages/dhcplease)
[<code>dhcplease-git</code>](https://aur.archlinux.org/packages/dhcplease-git).

Compile
-------
Simply <code>gcc dhcplease -o dhcplease</code> if you've only obtained the source code.

Usage
-----
<code>dhcplease \<DHCP Lease File\></code>

Example:
<code>dhcplease /var/lib/dhcpcd/dhcpcd-wlp3s0.lease</code>
```
Your IP Address        : 192.168.1.10
Your MAC Address       : 5c:90:a3:26:49:55
Subnet Mask            : 255.255.255.0
Router IP Address      : 192.168.1.1
DHCP Server IP Address : 192.168.1.1
DNS Server IP Address  : 222.222.128.1
DNS Server IP Address  : 222.222.128.2
Your Host Name         : mylaptop
IP Address Lease Time  : 12:00 h (43200 s)
Renewal Time Value     : 6:00 h (21600 s)
Rebinding Time Value   : 10:30 h (37800 s)
```

instead of:
<code>cat /var/lib/dhcpcd/dhcpcd-wlp3s0.lease</code>
```
�������L��6Qc�Sc5����@36�䀜���
```

Contribute
----------
You're complete free to fork this project and improve it on your own. I am also open to pull requests in order to get this utility to a higher level.

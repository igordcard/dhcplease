dhcplease
=========

dhcplease, or DHCP Lease Parser, is a command line application for GNU/Linux that parses a DHCP lease file right away. It is meant to be simple and straight to the point.

After no success in finding an existing application to parse one of my Arch Linux's dhcpcd lease files (/var/lib/dhcpcd/), forcing me to re-request a DHCP lease to capture and analyse the packets, I decided to do it myself. This is the result.

Get it
--------
You can clone this repository and then compile the program.
<code>git clone git@github.com:igordcard/dhcplease.git</code>

Arch Linux AUR package will be available soon.

Compile
-------
Simply <code>gcc dhcplease -o dhcplease</code>

Usage
-----
<code>dhcplease \<DHCP Lease File\></code>

Example:
<code>dhcplease /var/lib/dhcpcd/dhcpcd-wlp3s0.lease</code>

Contribute
----------
You're complete free to fork this project and improve it on your own. I am also open to pull requests in order to get this utility to a higher level.

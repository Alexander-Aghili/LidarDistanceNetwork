#/usr/bin/bash

sudo systemctl stop hostapd
sudo systemctl stop dnsmasq

sudo echo -e 'source-directory /etc/network/interfaces.d' >> /etc/network/interfaces

sudo head -n -3 /etc/dhcpcd.conf > tmp.conf && cp tmp.conf /etc/dhcpcd.conf

sudo service dhcpcd restart

echo 'Network stopped'
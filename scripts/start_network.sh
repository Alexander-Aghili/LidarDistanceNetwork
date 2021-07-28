#/usr/bin/bash
sudo head -n -1 /etc/network/interfaces > tmp.conf && cp tmp.conf /etc/network/interfaces

#DHCP config, sets this pi as host of network
sudo echo -e '\ninterface wlan0' >> /etc/dhcpcd.conf
sudo echo -e '\tstatic ip_address=192.168.10.1/24' >> /etc/dhcpcd.conf
sudo echo -e '\tnohook wpa_supplicant' >> /etc/dhcpcd.conf

sudo service dhcpcd restart
sudo systemctl start dnsmasq
sudo systemctl unmask hostapd
sudo systemctl enable hostapd
sudo systemctl start hostapd

echo 'Network started...'
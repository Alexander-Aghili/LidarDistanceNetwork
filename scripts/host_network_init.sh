#/usr/bin/bash

#Run the following command before running this script, it only needs to be run once though.
#sudo apt install dnsmasq hostapd


#DHCP config, sets this pi as host of network
sudo echo -e '\ninterface wlan0' >> /etc/dhcpcd.conf
sudo echo -e '\tstatic ip_address=192.168.10.1/24' >> /etc/dhcpcd.conf
sudo echo -e '\tnohook wpa_supplicant' >> /etc/dhcpcd.conf

sudo service dhcpcd restart

#DHCP clients config

#clear contents of dnsmasq file
truncate -s 0 /etc/dnsmasq.conf

sudo echo -e 'interface=wlan0' >> /etc/dnsmasq.conf
sudo echo -e 'dhcp-range=192.168.10.2,192.168.10.20,255.255.255.0,24h' >> /etc/dnsmasq.conf

sudo systemctl start dnsmasq



#hostapd conf
#clear
truncate -s 0 /etc/hostapd/hostapd.conf

sudo echo -e '\ninterface=wlan0\ndriver=nl80211\nssid=AlexNet\nhw_mode=g\nchannel=6\nmacaddr_acl=0\nauth_algs=1\nignore_broadcast_ssid=0\nwpa=2\nwpa_passphrase=alexmarcus0720\nwpa_key_mgmt=WPA-PSK\nwpa_pairwise=TKIP\nrsn_pairwise=CCMP' >> /etc/hostapd/hostapd.conf

sudo systemctl unmask hostapd
sudo systemctl enable hostapd
sudo systemctl start hostapd

echo -e 'Network initializtion comlete.\nNetwork started'
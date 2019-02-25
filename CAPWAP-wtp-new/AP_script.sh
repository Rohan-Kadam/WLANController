#!/bin/bash

function version_gt() { test "$(printf '%s\n' "$@" | sort -V | head -n 1)" != "$1"; }

echo "Checking Openssl..."
#=========================================================
#1.Check package
echo -n openssl: && dpkg-query -W --showformat='${Status}\n' openssl
dpkg_response="$(dpkg-query -W --showformat='${Status}\n' openssl)"

echo $dpkg_response
if [[ "$dpkg_response" == "install ok installed" ]]; then
        echo "Openssl package installed"
else
	echo "Please install Openssl (version <1.1.0) and re-run the script"
	exit
fi

#2.Check version
openssl_version="$(openssl version)"
curr_version=$(echo $nmcli_version | egrep -o '[0-9]'.'[0-9]'.'[0-9]')
reqd_version="1.0.2"

if version_gt $curr_version $reqd_version; then
        #echo "Openssl Version installed, $curr_version is greater than $reqd_version !"
        echo "Please remove current openssl version and install a version <1.1.0"
	exit
else
        #echo "Openssl Version installed, $curr_version is lesser than or equal to $reqd_version !"
        echo "Openssl Version OK"
fi

echo

#============================================================
#3.Destroy previously running process  

echo Start time
uptime -p
echo 3 Steps
echo =================
echo "(0/3)"
sudo killall AC
echo "(1/3)"
sudo killall WTP
echo "(2/3)"
sudo iw WTPWLan00 del
echo "(3/3)"
echo =================
echo Stop time
uptime -p

#============================================================
#4.Run WTP
echo "Listing all wireless interfaces"
iw dev
#Add a loop here for multiple deletions
read -p "Please enter wireless interface (if any listed above): " wireless_interface;
sudo iw ${wireless_interface} del
#sudo nmcli nm wifi off
#sudo nmcli nm status
sudo ./WTP .
echo WTP Command Executed
echo
#===========================================================
#5.Unblock Wifi radio
sudo ifconfig WTPWLan00 up
sudo ifconfig WTPWLan00 down
sudo rfkill unblock wifi

#===========================================================
#6.Check version
nmcli_version="$(nmcli --version)"
curr_version=$(echo $nmcli_version | egrep -o '[0-9]'.'[0-9]'.'[0-9]')
reqd_version="0.9.8"

if version_gt $curr_version $reqd_version; then
        #echo "Version installed, $curr_version is greater than $reqd_version !"
	sudo nmcli radio wifi off
	sudo nmcli all
else
        #echo "Version installed, $curr_version is lesser than or equal to $reqd_version !"
	sudo nmcli nm wifi on
	sudo nmcli nm status
fi

rfkill unblock all
#===========================================================
#8.Open Log
echo
iw dev
echo
sleep 2
tail -n 30 -f /var/log/wtp1.txt


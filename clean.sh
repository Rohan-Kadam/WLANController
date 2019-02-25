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


sudo chmod 777 /dev/video0 
sudo chmod 777 /dev/ttyACM0
stty -F /dev/ttyACM0 raw
stty -F /dev/ttyACM0 -echo -echoe -echok -hupcl
stty -F /dev/ttyACM0 115200
tcam-ctrl -p -s "Exposure=5000" 42614274
tcam-ctrl -p -s "Gain=96" 42614274
#tcam-ctrl -p -s "Brightness=16" 42614274

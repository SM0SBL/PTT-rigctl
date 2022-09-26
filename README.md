# PTT-rigctl
Connect an external PTT button using CTS or DSR on a serial port an connect to a rigctl compatible device
This program was written mainly to allow a physical PTT button when using wfview but should work with other rigctl compatible progams as well.

# Precompiled executable
I have attached a zip-file that should run on Windows10 64bit at least. 
Unpack the files somewhere on you windows disk and run the PTT-rigctl.exe

# Setup
There are two fields to be set up. One for the serial port used to detect when PTT is pushed and one to connect to the rigctl TCP port.

Start by setting up the serial port. Click "Update COM port list" if you com port is not visible in the drop down list. 
Next, set DSR or CTS as your pin to monitor.
Push the red "Connect" button in the COM ports section. It should turn green when your button is connected.
Test your connected PTT by observing the color of the PTT button on the application. If the button turns red when you push the button and green when you release it all is good.
If the behaviour of your button is inverted, check or uncheck the "Inv" box to invert the interpretation of the monitored pin.

When your serial attached PTT works you enter the ip address and port number of your rigctl interface. Normally this is the ip address of the local machine 127.0.0.1. The port is usually 4532 or 4533 but must be the correct port or the applicaiton will not be able to connect to your program.
Push the red "Connect" button to connect to you program. If the connection is successful the button turns green, if it does not turn green either your ip address or your port number is wrong or you might have a firewall stopping the connection.

Good luck!
Please send me a note if you use, or at least have tried, this little program and if it worked!

CXXFLAGS=-fPIC -Wall -g -c -I/usr/local/include/powerusb-1.0/ -Llib/64bit `pkg-config libusb-1.0 --cflags` -Wno-format-security
LDFLAGS=-Wall -L/usr/local/lib/ -Llib/64bit -g -Xlinker '-u libusb_init' -lhid -ldl `pkg-config libusb-1.0 --libs` -lusb-1.0 -lhid -lpowerusb -lusb-1.0 -lpthread -u libusb_init 


powerusb: powerusb.o
	g++ powerusb.o ${LDFLAGS} -o $@

powerusb.o: LinuxPowerUSB.hpp

clean:
	rm -f powerusb powerusb.o

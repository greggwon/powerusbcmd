PowerUSB Note :

	The source code has been compiled and tested on Ubuntu 20.04 with no issues. The usage
	of packages and installation of the packages hereby mentioned are with respect to Ubuntu.
	For other than Ubuntu based systems use the equivalent platform specific packages
	and install procedure.

PowerUSB usage procedure :
	
	Before making use of powerusb application make sure the following packages are installed :
		1. libudev
		2. libudev-dev
		3. libusb-1.0-0-dev

	To install the above packages, run the following commands in the terminal
		- sudo apt-get install libudev
		- sudo apt-get install libudev-dev
		- sudo apt-get install libusb-1.0-0-dev

Source Directory Structure :

	- udev (contains udev rules for Redhat and Debian based systems, place 99-powerusb.rules
          into /etc/udev/rules.d with respect to the system)

	- Makefile - use the 'make' command to build the powerusb executable

PowerUSB API :

	The following example shows how to invoke functions from the LinuxPowerUSB class APIs.

	#include "LinuxPowerUSB.hpp"

	int main(int argc, char* argv[])
	{
		LinuxPowerUSB p;
		try {
			p.Setup();

			... use functions from class definition ...

		} catch( LinuxPowerUSBError &ex ) {
			p.error( "%s: %s\n", argv[0], ex.what() );
		}
		return 2;
	}

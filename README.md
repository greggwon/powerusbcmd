Linux powerusb Notes :

	The source code has been compiled and tested on Ubuntu 20.04 with no issues. The usage
	of packages and installation of the packages hereby mentioned are with respect to Ubuntu.
	For other than Ubuntu based systems use the equivalent platform specific packages
	and install procedure.

LinuxPowerUSB class usage :
	
	Before making use of powerusb application make sure the following packages are installed :
		1. libudev
		2. libudev-dev
		3. libusb-1.0-0-dev

	To install the above packages, run the following commands in the terminal
		- sudo apt-get install libudev
		- sudo apt-get install libudev-dev
		- sudo apt-get install libusb-1.0-0-dev

Source Directory Structure :

	- Makefile - use the 'make' command to build the powerusb executable

LinuxPowerUSB API :

	The following example shows how to invoke functions from the LinuxPowerUSB class APIs.

	#include "LinuxPowerUSB.hpp"

	int main(int argc, char* argv[])
	{
		LinuxPowerUSB p;
		try {
			p.Setup();

			// ... use functions from class definition ...

			// p.setPortDefaultOff( int p );
			// p.setPortDefaultState( int p, bool how );
			// p.setDefaults( int state1, int state2, int state3 );
			// p.setPortStates( int state1, int state2, int state3 );
			// p.setPortState( int p, bool state );
			// p.setPortOn( int port );
			// p.setPortOff( int port );

			// Turn port 3 on if not on
			if( ! p.getPortState( 3 ) ) {
				p.setPortState(3, true );
			}

			// Make sure port 2 defaults to on
			p.setPortDefaultOn( 2 );

			// Make sure port 1 is off if on by default
			if( p.getPortDefaultState( 1 ) ) {
				p.setPortState( 1, false );
			}

		} catch( LinuxPowerUSBError &ex ) {
			p.error( "%s: %s\n", argv[0], ex.what() );
		}
		return 2;
	}

The 'powerusb' command line program :

	The included powerusb.cpp source code provides an example command line tool to test and
	use the LinuxPowerUSB class APIs to control a PowerUSB power strip.

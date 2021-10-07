#ifndef __LINUX_POWER_USB_H__
#define __LINUX_POWER_USB_H__

#include"powerusb.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <cstdarg>
#include <PwrUSBImp.h>

/**
 *  Exception for errors interacting with the USB interface and commanding changes
 */
class LinuxPowerUSBError : public virtual std::exception {
	std::string message;
public:
	template <typename T, typename... Args>
	LinuxPowerUSBError( T msg, Args... args ) : std::exception() {
		int len = std::snprintf( nullptr, 0, msg, args... );
		char buf[len+100];
		std::snprintf( buf, sizeof(buf), msg, args... );
		message = std::string(buf);
	}
	virtual const char* what() const throw() {
		return message.c_str();
	}
};

/**
 *  Exception for command line errors.
 */
class LinuxPowerUSBArgsError : public virtual LinuxPowerUSBError {
public:
	template <typename T, typename...Args>
	LinuxPowerUSBArgsError( T msg, Args...args ) : LinuxPowerUSBError(msg, args...) {}
};

/***
 *  Main class for interacting with the PowerUSB device via the HID library interface class, PowerUSB.
 */
class LinuxPowerUSB
{
	static bool debugging;

public:
	template <typename T, typename... Args>
	void report( T fmt, Args... args ) {
		int len = std::snprintf( nullptr, 0, fmt, args... );
		char buf[len+100];
		std::snprintf( buf, sizeof(buf), fmt, args... );
		printf( "%s", buf );
	}
private:

	template <typename T, typename... Args>
	void debug( T fmt, Args... args ) {
		int len = std::snprintf( nullptr, 0, fmt, args... );
		char buf[len+100];
		std::snprintf( buf, sizeof(buf), fmt, args... );
		if( debugging ) printf( "%s", buf );
	}

public :
	PowerUSB pwrusb;

	template <typename T, typename... Args>
	void error( T fmt, Args... args ) {
		int len = std::snprintf( nullptr, 0, fmt, args... );
		char buf[len+100];
		std::snprintf( buf, sizeof(buf), fmt, args... );
		fprintf( stderr, "%s", buf );
	}
	LinuxPowerUSB() :pwrusb()
	{
	}
	virtual ~LinuxPowerUSB() { pwrusb.close(); }

	void setDebug(bool how) { debugging = how; PowerUSB::debugging = how; }

	bool Setup() {
		init();
		return checkStatus();
	}

	void  init()
	{
		int i = -1;
		int v = pwrusb.init( &i );
		debug("init(&i) = %d max units, model=%d\n", v, i );
	}

	bool  checkStatus()
	{
		int status = pwrusb.checkStatus();
		debug("checkStatus = %d\n", status);
		if(!status)
		{
			throw LinuxPowerUSBError("LinuxPowerUSB Not Connected");
		}
		return status;
	}

	void setPortDefaultOn( int p ) {
		if( p > 0 && p < 4 ) {
			pwrusb.setDefaultState( 
				p == 1 ? true : PWRUSB_NO_CHANGE,
				p == 2 ? true : PWRUSB_NO_CHANGE,
				p == 3 ? true : PWRUSB_NO_CHANGE );
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}
	void setPortDefaultOff( int p ) {
		if( p > 0 && p < 4 ) {
			pwrusb.setDefaultState( 
				p == 1 ? false : PWRUSB_NO_CHANGE,
				p == 2 ? false : PWRUSB_NO_CHANGE,
				p == 3 ? false : PWRUSB_NO_CHANGE );
			return;
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}
	void setPortDefaultState( int p, bool how ) {
		if( p > 0 && p < 4 ) {
			pwrusb.setDefaultState( 
				p == 1 ? how : PWRUSB_NO_CHANGE,
				p == 2 ? how : PWRUSB_NO_CHANGE,
				p == 3 ? how : PWRUSB_NO_CHANGE );
			return;
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}
	void setDefaults( int state1, int state2, int state3 ) {
		pwrusb.setDefaultState( state1, state2, state3 );
	}

	void setPortStates( int state1, int state2, int state3 ) {
		pwrusb.setPort( state1, state2, state3 );
	}
	void setPortState( int p, bool state ) {
		if( p > 0 && p < 4 ) {
			pwrusb.setPort(
				p == 1 ? state : PWRUSB_NO_CHANGE,
				p == 2 ? state : PWRUSB_NO_CHANGE,
				p == 3 ? state : PWRUSB_NO_CHANGE );
			return;
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}
	void setPortOn( int port ) {
		setPortState( port, true );
	}
	int setCurrentDevice( int index ) {
		return pwrusb.selectDevice( index );
	}
	int getCurrentDevice() {
		return pwrusb.getCurrentDevice();
	}
	
	void setPortOff( int port ) {
		setPortState( port, false );
	}
	void getOutputStates( int states[7] ) {
		pwrusb.getOutputState( states );
	}

	int setOutputState(int outputs[]) {
		return pwrusb.setOutputState(outputs);
	}

	void setIODirection( int directions[7] ) {
		pwrusb.setIODirection( directions );
	}
	void getInputStates( int states[7] ) {
		pwrusb.getInputState( states );
	}
	int getVersion() {
		return pwrusb.getFirmwareVersion();
	}
	std::string getModel() {
		return pwrusb.getModelName();
	}
	bool powerCycleIn( int seconds ) {
		return pwrusb.powerCycle( seconds) > 0;
	}
	bool sendHeartBeat() {
		return pwrusb.sendHeartBeat() > 0;
	}
	bool closeAll() {
		return pwrusb.close() > 0;
	}
	bool setupPortToggle( int port, int onTimeSecs, int offTimeSecs ) {
		return pwrusb.generateClock( port, onTimeSecs, offTimeSecs ) > 0;
	}

	bool resetPowerUSB() {
		return pwrusb.resetBoard() > 0;
	}

	int setOverload( int overload ) {
		return pwrusb.setOverload( overload );
	}
	/**
	 * Return -1 = not connected, 0 = not in overload, >= 1 in overload
	 */
	int inOverload() {
		return pwrusb.getOverload();
	}
	/**
	 *  Starts watchdog in the PowerUSB. 
	 *  HbTimeSec:    Expected time for heartbeat
	 *  numHbMisses:  Number of accepted consecutive misses in heartbeat
	 *  resetTimeSec: Amount of time to switch off the computer outlet
	 */
	bool setWatchDogInterval( int heartBeatSecs, int numMissesAllowed, int resetTimeSecs ) {
		return pwrusb.startWatchdogTimer( heartBeatSecs, numMissesAllowed, resetTimeSecs ) > 0;
	}
	bool stopWatchDog() {
		return pwrusb.stopWatchdogTimer() > 0;
	}
	/**
	 *  Get the current state of Watchdog in PowerUSB
	 *  Return 0: watchdog is not running, 1: Watchdog is active, 2: In PowerCycle phase
	 */
	int getWatchdogStatus() {
		return pwrusb.getWatchdogStatus();
	}
	bool getPortState( int p ) { 
		int ps[3] = { p ==1, p ==2, p ==3 };
		if( p > 0 && p < 4 ) {
			if(pwrusb.readPortState(&ps[0], &ps[1], &ps[2]) > 0 ) {
				return(ps[p-1] );
			}
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}

	bool getPortDefaultState( int p ) {
		int ps[3] = { p == 1, p == 2, p == 3 };
		if( p > 0 && p < 4 ) {
			if( pwrusb.readDefaultPortState(&ps[0], &ps[1], &ps[2]) > 0 ) {
				return ps[p-1];
			}
		}
		throw LinuxPowerUSBArgsError("Invalid port number: %d", p );
	}

	void  reportStatus(int status, int port, bool verbose = true)
	{
		if( verbose ) report("Port %d = %s\n", port, status ? "On" : "Off");
		else report("%s\n", status ? "On" : "Off");
	}

	void  reportDefault(int status, int port, bool verbose = true)
	{
		if( verbose ) report("Default Port %d State = %s\n", port, status ? "On" : "Off");
		else report("%s\n", status ? "On" : "Off");
	}
};

bool LinuxPowerUSB::debugging;
#endif

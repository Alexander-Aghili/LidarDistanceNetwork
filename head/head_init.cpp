/*
 * Intial file run after boot by head node.
 * 
 * !Fix! to fix
 * 
 * Performed tasks:
 * - Checks network, and starts up network if necessary. Exits if there are errors starting network.
 * - 
 */

/*
 * Error Code Tracking:
 * 
 * Network:
 *  1: ssid cannot be found.
 *  2: terminal command "sudo iw dev wlan0 info" failed
 *  3: terminal command to run start_network.sh failed 
 * 
 * Lidar:
 * 	4: error starting serial port connection
 * 
 * Server:
 *  5: Cannot create Listening Socket
 * 	6: Multiple Client Connections Declined
 * 	7: Error Binding to Port
 * 	8: Could not listen to listening port
 * 	9: Could not accept client
 */

//C++ headers
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>

//C headers
#include <sys/ioctl.h>

//Custome headers
#include "log/log.h"
#include "lidar/lidar_stream.h"
#include "lidar/lidar_data.h"
#include "server/server.h"

/* Constant declarations */
const char* networkErrorMessage = "Program terminated with error code ";
const double spaceMargin = .2;
/* Constant declarations */


/* Method declartions */

//Check network methods
int checkNetwork();
std::string getHostapdNetworkName();
std::string getCurrentNetworkName();

//Lidar methods
void readLidarStream( int serial_port );
Lidar getLidarPing( int serial_port ) throw (int); //Ping refers to a single distance measurement

//Server methods
void hostServer();
void checkDistance( double* targetDistance, int* numClients, Server server, int* distanceValue );

//Helper related
std::string executeCommand( std::string command );

/* Method declartions */

//Global Variables
double realDistance = 0;
std::chrono::_V2::system_clock::time_point lastUpdateTime = std::chrono::high_resolution_clock::now();

int main() {

	//Start log of head node
	startLog();

	writeToLog("Detected head node properties. Head node startup sequence initiated: ");
	
	writeToLog("Performing network initialization");
	int network = checkNetwork();
	if (network != 0 ) {
		writeToLog("Closing due to network initialization error");
		std::cout << networkErrorMessage << network << std::endl;;
		return network;
	}
	writeToLog("Network status: Hosting");

	writeToLog("Initializing Lidar Device");
	int serial_port = initializeLidarDevice(B115200);
	if (serial_port == -1) {
		writeToLog("Closing due to lidar serial connection error");
		std::cout << networkErrorMessage << network << std::endl;
		return 4;
	}

	std::thread readingLidarThread(readLidarStream, serial_port);

	hostServer();

	readingLidarThread.join();

	writeToLog("End log");

	return 0;
}


/*
 * Gets hostapd ssid and current ssid
 * Checks network to see if the connected network name is the same as in the hostapd.
 * If it is different, runs start_network.sh.
 * 
 * @Return: int: 0 if successful network start/connection, error code if failed.
 */
int checkNetwork() {
	std::string hostapdNetworkName = getHostapdNetworkName();
	if (hostapdNetworkName == "1") return 1;

	std::string currentNetworkName = getCurrentNetworkName();
	if (currentNetworkName == "2") return 2;

	if (currentNetworkName == hostapdNetworkName) {
		return 0;
	}
	std::string startNetworkScript = "sudo /home/pi/scripts/start_network.sh";

	//If wifi is off this still returns 0 even though error pops up? !Fix!
	if (executeCommand(startNetworkScript).find("Network started...") != -1){
		return 0;
	} else {
		writeToLog("FATAL ERROR: COULD NOT START NETWORK. ENSURE start_network.sh IS IN THE INPUTTED FILE LOCATION.");
		return 3;
	}


}

//@Return: string: hostapd network name
//Checks for line with ssid and gets the network name. This is in case hostapd file was altered and it isn't at exact line
std::string getHostapdNetworkName() {
	std::string ssidLine = "ssid";
	std::string hostapdFile = "/etc/hostapd/hostapd.conf";
	std::ifstream file(hostapdFile.c_str());
	std::string line;

	try {
		while(getline(file, line)) {
			if (line.find(ssidLine, 0) != std::string::npos) {
				return line.substr(line.find("=") + 1); // Returns rest of line after = which should be the network name
			}
		}
		throw (1); //Error code
	} catch (int errorCode) {
		if (errorCode == 1) {
			writeToLog("FATAL ERROR: CANNOT FIND SSID IN HOSTAPD. ENSURE HOSTAPD IS INITIALIZED AND SET UP CORRECTLY.");
			return "1";
		}
	}
}

//@Return: string: ssid of network currently connected to
//executes command, gets output and return the ssid of network if found.
std::string getCurrentNetworkName()  {
	try {
		std::string outputOfCommand = executeCommand("sudo iw dev wlan0 info");
		if (outputOfCommand == "popen failed" || outputOfCommand.find("\n") == -1) throw 2;

		int locationOfSSID = outputOfCommand.find("ssid");
		if (locationOfSSID == -1) return ""; //Not connected to network

		//Below is parsing response to get network name
		std::string stringAfterSSID = outputOfCommand.substr(locationOfSSID + 5);
		std::string networkName = stringAfterSSID.substr(0, stringAfterSSID.find("\t") - 1);
		return networkName;
	} catch (int errorCode) {
		if (errorCode == 2) {
			writeToLog("FATAL ERROR: ERROR RUNNING COMMAND sudo iw dev wlan0 info ENSURE YOU HAVE PROPER PERMISSIONS.");
			return "2";
		} 
	}
}

/*
 * 
 * Reads entire lidar stream
 * Gets input buffer and waits until there is no bytes remaining.
 * Then takes calls getLidarPing which returns a Lidar object.
 * For now, we just print the data for debugging but later it will likely be used along side rotation data to
 * estimate the location of objects in space.
 * 
 * @param serial_port for interaction with serial data
 */
void readLidarStream( int serial_port ) {
	bool fatalError = false;
	int errorsInARow=0;

	while (!fatalError){
		try {
			int inputBufferBytesRemaining;

			if (ioctl(serial_port, FIONREAD, &inputBufferBytesRemaining) == -1) {
				writeToLog("ERROR: Cannot recieve input buffer");
				throw errno;
			}

			while (inputBufferBytesRemaining > 0) {
				serialGetchar(serial_port); //Get the bytes until no bytes remaining in the input buffer
				inputBufferBytesRemaining--;
			}

			errorsInARow = 0;

			Lidar ping = getLidarPing(serial_port);
			double tempDis = ping.getDistance();
			if (tempDis < 15) {
				realDistance = tempDis;
				lastUpdateTime = std::chrono::high_resolution_clock::now();
			}

		} catch (int error) {
			serialFlush(serial_port); //Flush serial port
			serialClose(serial_port); //Closer serial port
			writeToLog("Reconnecting Lidar Device", true);
			serial_port = initializeLidarDevice(B115200); //Reinit serial port

			errorsInARow++;
			if(errorsInARow > 10) {
				fatalError = true;
			}
			
		}
	}

	writeToLog("FATAL ERROR OCCURED WHILE READING LIDAR STREAM. ENDING STREAM");
	serialClose(serial_port);
}

/*
 * 
 * 8 bytes come from each data stream are below. For more detail read: 
 * https://s3-us-west-2.amazonaws.com/files.seeedstudio.com/products/101990656/res/SJ-PM-TF-Luna+A01+Product+Manual.pdf
 * 
 * byte 0: 0x59 header
 * byte 1: 0x59 header
 * byte 2: Dist_L distance value low 8 bits
 * byte 3: Dist_H distance value high 8 bits
 * byte 4: Strength_L low 8 bits
 * byte 5: Strength_H high 8 bits
 * byte 6: Temp_L low 8 bits
 * byte 7: Temp_H high  bits
 * byte 8: Check sum
 * 
 * If your device uses something other than 8 bits make sure to change uint8_t to the proper bit per byte amount
 * Also adjust to the proper bytes.
 * 
 * This method is called once the input buffer is 0, so it start to read the bytes in a for loop
 * The first byte should be 0x59 and a check is performed at the beginning. 
 * Throws error if bytes[0] is not 0x59 because data will be incorrect.
 * Uses the chart above to calculate the distance, strength, and tempurature and returns a Lidar object
 * 
 * @param serial_port the serial port used for connecting to lidar device
 * @throws errno: error is byte stream is not correct for lidar device
 * @return Lidar object that container distance (in meters), strength, and tempurature (in centigrade)
 */

Lidar getLidarPing( int serial_port ) throw (int) {
	int numBytes = 8;
	uint8_t bytes[numBytes]; //Array of 8 bytes for data stream

	for (int i = 0; i < numBytes + 1; i++) {
		bytes[i] = serialGetchar(serial_port);
	}

	//true if bytes in stream wont be correct from Lidar device, throws bytes[0]
	if (bytes[0] != 0x59 || bytes[1] != 0x59) { 
		writeToLog("WARNING: Byte stream faulty", true);
		throw errno;
	}

	double distance = ((double)(bytes[2] + bytes[3] * 256))/100; //bytes[2] + bytes[3] * 256 gives distance in cm, converted to m with /100
	int strength = bytes[4] + bytes[5] * 256;
	double tempurature = ((double)(bytes[6] + bytes[7] * 256))/8 - 256;


	return Lidar(distance, strength, tempurature);
} 

/*
 * Starts hosting server.
 * @init targetDistance double default 2.0 m
 * @init numClients int default 0 clients
 * @init clientSocket int[5] default null Socket Number of clients.
 * 
 * Memory addr of inits is sent into Server class which starts a new thread.
 * Then distance value is initialized and a while true loop starts that checks if the distance picked up
 * is too far to, too close to, or in the target distance
 */
void hostServer() {
	double targetDistance = 2.0; //2 m start
	int numClients = 0;
	int clientSocket[5];
	std::cout << &clientSocket[0] << std::endl; //debug

	Server server(&targetDistance, &numClients, &clientSocket[0]);
	std::thread serverThread(server);

	int distanceValue = 3; //1 too close 2 too far 3 in position
	while (true)
		checkDistance(&targetDistance, &numClients, server, &distanceValue);

	serverThread.join();
}

/* 
 * @param double* targetDistance: takes in the memory addr of targetDistance to reference when comparing to real distance.
 * @param int* numClients: maybe used later, irrelevant right now
 * @param Server server: is the server class sent into the thread, used to send message to host
 * @param int* distanceValue: Used to reference if client is too far to, too close to, or in the targetDistance.
 * 
 * Timing is used to determine if data is reliable. If data takes longer than .0125 seconds from when it was read to check it is determined unreliable and skipped.
 * Method waits until data read is reliable.
 * 
 * If data is reliable two ifs to check real distance relative to target distance. If it is within the target distance, it doesn't do anything
 * However, if it is not, then it sends that data to the host for, also to the clients later hopefully. 
 *
 */
void checkDistance( double* targetDistance, int* numClients, Server server, int* distanceValue) {
	std::chrono::_V2::system_clock::time_point tempTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = tempTime - lastUpdateTime;
	if (duration.count() > .0125f) {
		return; //Unreliable data
	}
	bool distanceValueChanged = false;

	if (*targetDistance > (realDistance + spaceMargin)) {
		*distanceValue = 1;
	} else  if (*targetDistance < (realDistance - spaceMargin)) {
		*distanceValue = 2;
	} else {
		if (*distanceValue != 3) {
			*distanceValue = 3;
			distanceValueChanged = true;
		}
	}

	if (*numClients > 0 && distanceValueChanged || *numClients > 0 && *distanceValue != 3) {
		std::string message = std::to_string((realDistance- *targetDistance)) + "\n";
		server.sendMessageToHost(message);
	}

}



/* Runs command and returns the output */
std::string executeCommand(std::string command) {
   char buffer[128];
   std::string result = "";

   // Open pipe to file
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed";
   }

   // read till end of process:
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   pclose(pipe);
   return result;
}

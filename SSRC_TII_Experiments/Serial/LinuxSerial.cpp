/**============================================================================
Name        : LinuxSerial.h
Created on  : 10.12.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : LinuxSerial
============================================================================**/

#include "LinuxSerial.h"
#include "SerialPort.h"

namespace Tests
{
    using namespace mn::CppLinuxSerial;

    void Test1() {
        // Create serial port object and open serial port at 57600 buad, 8 data bits, no parity bit, one stop bit (8n1),
        // and no flow control
        SerialPort serialPort("/dev/ttyUSB0",
                              BaudRate::B_115200,
                              NumDataBits::EIGHT,
                              Parity::NONE,
                              NumStopBits::ONE);
        // Use SerialPort serialPort("/dev/ttyACM0", 13000); instead if you want to provide a custom baud rate

        serialPort.SetTimeout(-1); // Block when reading until any data is received
        serialPort.Open();

        // Write some ASCII data
        serialPort.Write("ps axf");
        // serialPort.SetTimeout(1000); // Block when reading for 1000ms

        // Read some data back (will block until at least 1 byte is received due to the SetTimeout(-1) call above)
        std::string readData;

        serialPort.Read(readData);
        std::cout << readData << std::endl;


        serialPort.Read(readData);
        std::cout << readData << std::endl;


        // Close the serial port
        serialPort.Close();
    }
}

void LinuxSerial::TestAll() {
    Tests::Test1();
}
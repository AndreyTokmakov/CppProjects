/**============================================================================
Name        : LibSerial.cpp
Created on  : 10.12.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : LibSerial
============================================================================**/

#include "LibSerial.h"

#include <iostream>

#include <SerialPort.h>
#include <SerialStream.h>

using namespace LibSerial ;

namespace LibSerialTests
{

    void Test()
    {
        // Create and open the serial port for communication.
        // SerialPort   my_serial_port( "/dev/ttyS0" );
        SerialStream my_serial_stream( "/dev/ttyUSB0" ) ;
        std::cout << "Opened\n";

        // my_serial_port.SetBaudRate( BAUD_115200 );
        my_serial_stream.SetBaudRate(LibSerial::BaudRate::BAUD_115200 );
        my_serial_stream.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
        my_serial_stream.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
        // my_serial_stream.Bit(LibSerial::StopBits::STOP_BITS_1);

        // You can easily write strings.
        std::string cmd = "pwd";
        my_serial_stream << cmd << std::endl ;

        // Read a whole array of data from the serial port.
        const int BUFFER_SIZE = 256;
        char buffer[BUFFER_SIZE];

        my_serial_stream.read(buffer, BUFFER_SIZE);
        std::cout << buffer << std::endl;

        my_serial_stream.read(buffer, BUFFER_SIZE);
        std::cout << buffer << std::endl;

        my_serial_stream.Close();
        std::cout << "Closed\n";
    }
};

void LibSerial::TestAll()
{
    LibSerialTests::Test();
};
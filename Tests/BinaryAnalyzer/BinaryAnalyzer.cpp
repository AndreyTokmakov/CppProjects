/**============================================================================
Name        : BinaryAnalyzer.h
Created on  : 21.03.2023
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : BinaryAnalyzer
============================================================================**/

#include "BinaryAnalyzer.h"

// https://habr.com/ru/company/inforion/blog/460247/

/**     * * * * readelf * * * *

# ELF Header:
    readelf -h Tests

# Section Headers:
    readelf -S -W Tests

# Read information about segments and sections
    readelf -l -W Tests

**/



/**     * * * * objdump * * * *

# For information about the compiler and assembly, see the .comment and .note sections:

    objdump -s --section .comment Tests

**/



/**  * * * * NM * * * *

# Тут имена функций выводятся в манглированном виде
    nm -D file_name

# Тут имена функций выводятся в читаемом виде
    nm -D --demangle file_name

**/

namespace BinaryAnalyzer
{

};

void BinaryAnalyzer::TestAll()
{
};

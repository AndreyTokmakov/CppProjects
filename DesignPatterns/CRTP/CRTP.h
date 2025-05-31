/**============================================================================
Name        : CRTP.h
Created on  : 09.04.2021
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CRTP src
============================================================================**/

#ifndef CRTP_DESIGN_PATTERN_TESTS__H_
#define CRTP_DESIGN_PATTERN_TESTS__H_

namespace CRTP {
	void Test();
}

namespace CRTP::CRTP_Cloneable {
    void Test();
}

namespace CRTP::ObjectCounter {
    void Test();
}

namespace CRTP::Policy_Based_Design {
    void TestAll();
}

namespace CRTP::Simple_Comparable_Base {
    void TestAll();
}

#endif /* CRTP_DESIGN_PATTERN_TESTS__H_ */



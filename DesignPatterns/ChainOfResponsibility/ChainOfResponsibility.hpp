/**============================================================================
Name        : ChainOfResponsibility.hpp
Created on  : 19.04.2020
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Chain of pesponsibility pattern
============================================================================**/

#ifndef CHAIN_OF_RESPONSIBILITY_PATTERN_TESTS__H_
#define CHAIN_OF_RESPONSIBILITY_PATTERN_TESTS__H_

namespace chain_of_responsibility
{
	void TestAll();
	namespace handlers_1 { void testAll(); }
	namespace handlers_2 { void testAll(); }
	namespace frequency_validator { void testAll(); }
	namespace frequency_validator_pipeline { void testAll(); }
	namespace packet_processing_pipeline { void testAll(); }
	namespace packet_processing_pipeline_2 { void testAll(); }
}

#endif /* CHAIN_OF_RESPONSIBILITY_PATTERN_TESTS__H_ */



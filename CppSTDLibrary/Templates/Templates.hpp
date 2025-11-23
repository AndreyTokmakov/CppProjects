/**============================================================================
Name        : Templates.h
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Templates src
============================================================================**/

#ifndef TEMPLATES_TESTS__H_
#define TEMPLATES_TESTS__H_

namespace Templates
{
	void TestAll();

	namespace NTTP { void TestAll(); };
	namespace PackIndexing { void TestAll(); }
	namespace SFINAE { void TestAll(); }
	namespace TemplateSpecialization { void TestAll(); }
	namespace FoldExpressions { void TestAll(); }
	namespace ConditionalExplicit { void TestAll(); }
	namespace TypeListChecker { void TestAll(); }
	namespace IndexSequence { void TestAll(); }
};

#endif /* TEMPLATES_TESTS__H_ */

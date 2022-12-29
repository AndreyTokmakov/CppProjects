#include <iostream>
#include "Page.h"

namespace Proxy_Pattern_Tests {

	Page::Page() {
		std::cout << __FUNCTION__ << std::endl;
	}

	Page::~Page() {
		std::cout << std::endl << __FUNCTION__ << std::endl;
	}

	void Page::displayText() {
		std::cout << "Displaying text." << std::endl;
	}

	void Page::displayImage() {
		std::cout << "Displaying an image." << std::endl;
	}

	void Page::displayVideo() {
		std::cout << "Displaying a video." << std::endl;
	}

	void Page::displayEmpty() {
		std::cout << "Sorry nothing to display." << std::endl;
	}

}
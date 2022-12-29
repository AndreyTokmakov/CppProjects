#include <iostream>
#include "Document.h"
#include "PageProxy.h"

namespace Proxy_Pattern_Tests {

	Document::Document() {
		std::cout << __FUNCTION__ << std::endl;
		initProxy();
	}

	Document::~Document() {
		std::cout << __FUNCTION__ << std::endl;
	}

	void Document::open(int pageNumber) {
		_proxy.displayContent(pageNumber);
	}

	void Document::initProxy() {
	}

	void Document::changeToPage(int pageNumber) {
		_proxy.displayContent(pageNumber);
	}

}
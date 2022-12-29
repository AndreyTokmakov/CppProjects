//============================================================================
// Name        : Flyweight.cpp
// Created on  : 12.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Flyweight pattern src
//============================================================================

#include "Flyweight.h"

#include <iostream>
#include <string>

namespace Flyweight::FlyweightCharacter {

	ConcreteCharacter::ConcreteCharacter(char aSymbol, int aPointSize)
	{
		mSymbol = aSymbol;
		mWidth = 120;
		mHeight = 100;
		mAscent = 70;
		mDescent = 0;
		mPointSize = aPointSize;
		std::cout << __FUNCTION__ << "[" << mSymbol << ", " << mPointSize << " ]" << std::endl;
	}

	void ConcreteCharacter::display() const noexcept {
		std::cout << mSymbol << " ( PointSize " << mPointSize << " )" << std::endl;
	}


	// TODO: Make emplace!!!! vs mCharacters[aKey] = std::make_unique<const ConcreteCharacter>(aKey, POINT_SIZE);

	template <const int POINT_SIZE>
	const Character& CharacterFactory<POINT_SIZE>::getCharacter(char aKey) {
		if (const auto it = mCharacters.find(aKey); mCharacters.end() == it) {
			mCharacters[aKey] = std::make_unique<const ConcreteCharacter>(aKey, POINT_SIZE);
			return *mCharacters[aKey];
		}
		else {
            // std::cout << "Use exising\n";
			return *it->second;
		}
	}

	void Test() {
		std::string document = "AAZZBBZB";

		CharacterFactory<12> characterFactory;

		for (auto it : document) {
			auto&& character = characterFactory.getCharacter(it);
			character.display();
		}
	}
}

void Flyweight::TestDriver()
{
	FlyweightCharacter::Test();

	// FlyweightIcon::Test();
}

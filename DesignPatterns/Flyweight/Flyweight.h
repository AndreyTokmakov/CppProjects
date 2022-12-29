//============================================================================
// Name        : Flyweight.h
// Created on  : 12.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Command pattern src
//============================================================================

#ifndef FLYWEIGHT_PATTERN__TESTS_H__
#define FLYWEIGHT_PATTERN__TESTS_H__

#include <unordered_map>
#include <memory>

namespace Flyweight::FlyweightCharacter {

	class Character {
	public:
		char mSymbol;
		int  mWidth;
		int  mHeight;
		int  mAscent;
		int  mDescent;
		int  mPointSize;

	public:
		virtual ~Character() = default;
		virtual void display() const noexcept = 0;
	};


	class ConcreteCharacter : public Character {
	public:
		// Constructor
		ConcreteCharacter(char aSymbol, int aPointSize);

		// from Character
		virtual void display() const noexcept override;
	};


	/** Flyweight factory: **/
	template <const int POINT_SIZE>
	class CharacterFactory {
	private:
        using Characters = std::unordered_map<char, std::unique_ptr<const Character>>;
		Characters mCharacters;

	public:
		const Character& getCharacter(char aKey);
	};
}

namespace Flyweight::FlyweightIcon {
	void Test();
}

namespace Flyweight {
	void TestDriver();
}

#endif // !(FLYWEIGHT_PATTERN__TESTS_H__)
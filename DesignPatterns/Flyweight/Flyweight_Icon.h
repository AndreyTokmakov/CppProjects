//============================================================================
// Name        : FlyweightIcon.h
// Created on  : 12.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Command pattern src
//============================================================================

#ifndef ICON_FLYWEIGHT_PATTERN__TESTS_H__
#define ICON_FLYWEIGHT_PATTERN__TESTS_H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace Flyweight::FlyweightIcon {

	class Icon {
	private:
		inline static const std::string goIconName     = "go";
		inline static const std::string stopIconName   = "stop";
		inline static const std::string selectIconName = "select";
		inline static const std::string undoIconName   = "go";

	protected:
		std::string name;
		size_t  width;
		size_t  height;

	public:
		template<typename S>
		explicit Icon(S&& name); 

		inline const std::string& getName() const noexcept {
			return this->name;
		}
		void draw(size_t x, size_t y);
	};


	// FlyweightFactory final class:
	class FlyweightFactory final {
	private:
		std::unordered_map<std::string, std::shared_ptr<Icon>> icons;

	public:
		static FlyweightFactory* getFactory();

	public:
		template<typename S>
		std::shared_ptr<Icon> getIcon(S&& iconName);

		//std::shared_ptr<Icon> getIcon(const std::string& iconName);

		void reportTheIcons();

	private:
		// Seal constructor
		FlyweightFactory();
		// Seal copy constructor
		FlyweightFactory(const FlyweightFactory& root) = default;
		// Disable copy assign
		FlyweightFactory operator=(const FlyweightFactory&) = delete;
	};


	// DialogBox abstact class:
	class DialogBox {
	protected:
		std::vector<std::shared_ptr<Icon>> icons;
		int iconsOriginX;
		int iconsOriginY;
		int iconsXIncrement;
		inline const static int ICON_MAX = 3;

	public:
		DialogBox(int x, int y, int incr);
		virtual void draw() = 0;
	};


	// FileSelection final class:
	class FileSelection: public DialogBox {
	public:
		FileSelection(const std::shared_ptr<Icon>& first,
			          const std::shared_ptr<Icon>& second,
				      const std::shared_ptr<Icon>& third);
		void draw() override;
	};

	// CommitTransaction final class:
	class CommitTransaction : public DialogBox {
	public:
		CommitTransaction(std::shared_ptr<Icon> first,
						  std::shared_ptr<Icon> second,
						  std::shared_ptr<Icon> third);
		void draw() override;
	};

}

#endif // ICON_FLYWEIGHT_PATTERN__TESTS_H__
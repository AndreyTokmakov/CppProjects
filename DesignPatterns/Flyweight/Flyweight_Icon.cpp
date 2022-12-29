//============================================================================
// Name        : FlyweightIcon.cpp
// Created on  : 12.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Flyweight pattern src
//============================================================================

#include "Flyweight.h"

#include <iostream>
#include <string>
#include "Flyweight_Icon.h"

namespace Flyweight::FlyweightIcon {

	template<typename S>
	Icon::Icon(S&& name) : name(std::forward<S>(name)) {
		if (Icon::goIconName == name) {
			this->width = 20;
			this->height = 20;
		}
		else if (Icon::stopIconName == name) {
			this->width = 40;
			this->height = 40;
        }
        else if (Icon::selectIconName == name) {
			this->width = 60;
			this->height = 60;
        }
        else if (Icon::undoIconName == name) {
			this->width = 30;
			this->height = 30;
		}
		else {
			// TODO: Handle this somehow
		}
		std::cout << "Icon '" << name << "' constructed" << std::endl;
	}

	void Icon::draw(size_t x, size_t y) {
		std::cout << "   drawing " << name << ": upper left (" << x << "," << y <<
			") - lower right (" << x + width << "," << y + height << ")" << std::endl;
	}

	//-------------------------------------------------------------------------------------//

	FlyweightFactory::FlyweightFactory() {
		this->icons.reserve(4);
	}

	FlyweightFactory* FlyweightFactory::getFactory() {
		static FlyweightFactory theSingleInstance;
		// Return with RVO optimization.... i hope
		return &theSingleInstance;
	}

	template<typename S>
	std::shared_ptr<Icon> FlyweightFactory::getIcon(S&& iconName) {
		if (auto iter = this->icons.find(std::forward<S>(iconName)); this->icons.end() != iter)
			return iter->second;
		return this->icons.emplace(std::forward<S>(iconName), 
			std::make_shared<Icon>(std::forward<S>(iconName))).first->second;
	}

	/*
	std::shared_ptr<Icon> FlyweightFactory::getIcon(const std::string& iconName) {
		if (auto iter = this->icons.find(iconName); this->icons.end() != iter)
			return iter->second;
		return this->icons.emplace(iconName, std::make_shared<Icon>(iconName)).first->second;
	}
	*/

	void FlyweightFactory::reportTheIcons() {
		std::cout << "\nActive Flyweights: " << std::endl;
		for (const auto&[k, v] : this->icons)
			std::cout << "   [ " << k << " ]" << std::endl;
	}

	//-------------------------------------------------------------------------------------//

	DialogBox::DialogBox(int x, int y, int incr) :
		iconsOriginX(x), iconsOriginY(y), iconsXIncrement(incr) {
		// Reserve some space.
		this->icons.reserve(DialogBox::ICON_MAX);
	}

	//-------------------------------------------------------------------------------------//

	FileSelection::FileSelection(const std::shared_ptr<Icon>& first,
								 const std::shared_ptr<Icon>& second,
								 const std::shared_ptr<Icon>& third) : DialogBox(100, 100, 100)
	{
		icons.push_back(first);
		icons.push_back(second);
		icons.push_back(third);
	}

	void FileSelection::draw() {
		std::cout << "drawing FileSelection:" << std::endl;
		for (int i = 0; i < 3; i++)
			icons[i]->draw(iconsOriginX + (i *iconsXIncrement), iconsOriginY);
	}

	//-------------------------------------------------------------------------------------//

	CommitTransaction::CommitTransaction(std::shared_ptr<Icon> first,
										 std::shared_ptr<Icon> second,
										 std::shared_ptr<Icon> third) : DialogBox(150, 150, 150)
	{
		icons.push_back(first);
		icons.push_back(second);
		icons.push_back(third);
	}

	void CommitTransaction::draw() {
		std::cout << "drawing CommitTransaction:" << std::endl;
		for (int i = 0; i < 3; i++)
			icons[i]->draw(iconsOriginX + (i *iconsXIncrement), iconsOriginY);
	}

	//-------------------------------------------------------------------------------------//
	
	void Test() {
		FlyweightFactory* factory = FlyweightFactory::getFactory();

		/*
		std::shared_ptr<Icon> goIcon = factory->getIcon("go");
		std::shared_ptr<Icon> stopIcon = factory->getIcon("stop");
		std::shared_ptr<Icon> selectIcon = factory->getIcon("select");
		std::shared_ptr<FileSelection> fileDialog = std::make_shared<FileSelection>(goIcon, stopIcon, selectIcon);
		*/
		std::vector<std::shared_ptr<DialogBox>> dialogs;
		dialogs.push_back(std::make_shared<FileSelection>(
                factory->getIcon("go"),
                factory->getIcon("stop"),
                factory->getIcon("select")));

		dialogs.push_back(std::make_shared<CommitTransaction>(
                factory->getIcon("select"),
                factory->getIcon("stop"),
                factory->getIcon("undo")));


		for (const auto& D : dialogs)
			D->draw();

		factory->reportTheIcons();
	}
}
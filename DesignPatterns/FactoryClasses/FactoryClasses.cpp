/**============================================================================
Name        : FactoryClasses.cpp
Created on  : 09.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "FactoryClasses.h"

#include <iostream>
#include <memory>
#include <string_view>
#include <unordered_map>

namespace FactoryClasses
{
    struct IDocument
    {
        virtual ~IDocument() = default;
        virtual bool Open() = 0;
    };

    struct ODTDocument final : public IDocument
    {
        std::string_view filePath;

        explicit ODTDocument(const std::string_view path): filePath { path } {
            //std::cout << __PRETTY_FUNCTION__ << '(' << filePath << ")\n";
        }

        ~ODTDocument() override {
            //std::cout << __PRETTY_FUNCTION__ << '(' << filePath << ")\n";
        }

        bool Open() override
        {
            std::cout << "Opening ODT file" << filePath << '\n';
            return true;
        }
    };


    struct TXTDocument final : public IDocument
    {
        std::string_view filePath;

        explicit TXTDocument(const std::string_view path): filePath { path } {
            //std::cout << __PRETTY_FUNCTION__ << '(' << filePath << ")\n";
        }

        ~TXTDocument() override {
            //std::cout << __PRETTY_FUNCTION__ << '(' << filePath << ")\n";
        }

        bool Open() override
        {
            std::cout << "Opening TXT file" << filePath << '\n';
            return true;
        }
    };

    struct DocumentOpenersFactory
    {
        using OpenerType = std::unique_ptr<IDocument>;
        using ConcreteOpener = OpenerType (*)(std::string_view);

        std::unordered_map<std::string_view, ConcreteOpener> openerByExtension;

        void Register(std::string_view extension, ConcreteOpener opener)
        {
            openerByExtension.emplace(extension, opener);
        }

        OpenerType open(const std::string_view path) const
        {
            if (const size_t dotPos = path.find_last_of('.'); dotPos != std::string_view::npos)
            {
                const std::string_view extension = path.substr(dotPos + 1);
                return openerByExtension.at(extension)(path);
            }
            throw std::invalid_argument { "Trying to open a file with no extension" };
        }
    };
}

void FactoryClasses::TestAll()
{
    using OpenerType = DocumentOpenersFactory::OpenerType;

    DocumentOpenersFactory openerFactory = DocumentOpenersFactory {};
    openerFactory.Register("odt", [](std::string_view path) -> OpenerType {
        return std::make_unique<ODTDocument>(path);
    });
    openerFactory.Register("txt", [](std::string_view path) -> OpenerType {
       return std::make_unique<TXTDocument>(path);
    });

    const OpenerType odtOpener = openerFactory.open(".odt");
    odtOpener->Open();

    const OpenerType txtOpener = openerFactory.open(".txt");
    txtOpener->Open();
}
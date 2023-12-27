/**============================================================================
Name        : NVI_SimpleExample.cpp
Created on  : 27.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : NVI_SimpleExample.cpp
============================================================================**/

#include "NVI_SimpleExample.h"
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>

struct ReaderWriterLock
{
    void acquire() {}
    void release() {}
};

struct SomeComplexDataType
{
    bool check_invariants() {}
};


class Base
{
    mutable ReaderWriterLock lock;
    SomeComplexDataType data;

public:

    /** non-virtual **/
    void read_from(std::istream& stream)
    {
        lock.acquire();
        assert(data.check_invariants());

        read_from_impl(stream);

        assert(data.check_invariants());
        lock.release();
    }

    /** non-virtual **/
    void write_to(std::ostream& stream) const
    {
        lock.acquire();
        write_to_impl(stream);
        lock.release();
    }

    /** Virtual because Base is a polymorphic base class.**/
    virtual ~Base() = default;

private:

    virtual void read_from_impl( std::istream & ) = 0;
    virtual void write_to_impl( std::ostream & ) const = 0;
};

class XMLReaderWriter : public Base
{
    void read_from_impl(std::istream&) override {
        std::cout << "XMLReaderWriter::read_from_impl()" << std::endl;
    }

    void write_to_impl(std::ostream&) const override {
        std::cout << "XMLReaderWriter::write_to_impl()" << std::endl;
    }
};

class TextReaderWriter : public Base
{
    void read_from_impl(std::istream&) override {
        std::cout << "TextReaderWriter::read_from_impl()" << std::endl;
    }

    void write_to_impl(std::ostream&) const override {
        std::cout << "TextReaderWriter::write_to_impl()" << std::endl;
    }
};

void clientCode(Base* writer)
{
    writer->read_from(std::cin);
    writer->write_to(std::cout);
}

void NVI_SimpleExample::TestAll()
{
    std::unique_ptr<Base> xmlWriter = std::make_unique<XMLReaderWriter>();
    std::unique_ptr<Base> textWriter = std::make_unique<TextReaderWriter>();

    clientCode(xmlWriter.get());
    clientCode(textWriter.get());
}
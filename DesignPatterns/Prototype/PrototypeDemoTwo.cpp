/**============================================================================
Name        : PrototypeDemoType.cpp
Created on  : October 26, 2020
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : PrototypeDemoType
============================================================================**/

#include <iostream>
#include <memory>
#include <utility>
#include <unordered_map>
#include <format>

/** Record is the base Prototype */
struct Record
{
    virtual ~Record() = default;
    virtual void print() = 0;
    virtual std::unique_ptr<Record> clone() = 0;
};

/** CarRecord is a Concrete Prototype */
class CarRecord : public Record
{
private:
    std::string name;
    int id {0};

public:
    CarRecord(std::string carName, int ID):
            name { std::move(carName) }, id(ID) {
    }

    void print() override
    {
        std::cout << std::format("(CarRecord: Name: {}, id: {})", name, id) << std::endl;
    }

    std::unique_ptr<Record> clone() override
    {
        return std::make_unique<CarRecord>(*this);
    }
};

/** BikeRecord is the Concrete Prototype */
class BikeRecord : public Record
{
private:
    std::string name;
    int id {0};

public:
    BikeRecord(std::string bikeName, int ID) :
            name { std::move(bikeName) }, id(ID) {
    }

    void print() override
    {
        std::cout << std::format("(BikeRecord: Name: {}, id: {})", name, id) << std::endl;
    }

    std::unique_ptr<Record> clone() override
    {
        return std::make_unique<BikeRecord>(*this);
    }
};

/** PersonRecord is the Concrete Prototype */
class PersonRecord : public Record
{
private:
    std::string name;
    int age {0};

public:
    PersonRecord(std::string personName, int age) :
            name { std::move(personName) }, age (age) {
    }

    void print() override
    {
        std::cout << std::format("(PersonRecord: Name: {}, Age: {})", name, age) << std::endl;
    }

    std::unique_ptr<Record> clone() override
    {
        return std::make_unique<PersonRecord>(*this);
    }
};

/** Opaque record type, avoids exposing concrete implementations */
enum class RecordType
{
    Car,
    Bike,
    Person
};

template<>
struct std::hash<RecordType> {
    size_t operator()(const RecordType type) const {
        return std::hash<int>()(static_cast<int>(type));
    }
};

/** RecordFactory is the client */
class RecordFactory
{
private:
    std::unordered_map<RecordType, std::unique_ptr<Record>, std::hash<RecordType>> cache;

public:
    RecordFactory()
    {
        cache[RecordType::Car]    = std::make_unique<CarRecord>("Ferrari", 5050);
        cache[RecordType::Bike]   = std::make_unique<BikeRecord>("Yamaha", 2525);
        cache[RecordType::Person] = std::make_unique<PersonRecord>("Tom", 25);
    }

    std::unique_ptr<Record> createRecord(RecordType recordType) const
    {
        if (const auto iter = cache.find(recordType); cache.end() != iter)
            return iter->second->clone();
        return {};
    }
};


void PrototypeDemoType_Test()
{
    RecordFactory recordFactory;

    auto record = recordFactory.createRecord(RecordType::Car);
    record->print();

    record = recordFactory.createRecord(RecordType::Bike);
    record->print();

    record = recordFactory.createRecord(RecordType::Person);
    record->print();
}
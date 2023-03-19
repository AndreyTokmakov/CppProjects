/**============================================================================
Name        : LambdaObserver.h
Created on  : 18.03.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : LambdaObserver
============================================================================**/

#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <set>


namespace LambdaObserver
{
    template< typename Subject, typename StateTag >
    class Observer
    {
    public:
        using OnUpdate = void (*)(Subject const&,StateTag);

        // No virtual destructor necessary

        explicit Observer(OnUpdate onUpdate) : onUpdate { std::move(onUpdate) } {
            // Possibly respond on an invalid/empty std::function instance
        }

        // Non-virtual update function
        void update(Subject const& subject, StateTag property) {
            onUpdate(subject, property);
        }

    private:
        OnUpdate onUpdate;
    };


    class Person
    {
    public:
        enum StateChange
        {
            forenameChanged,
            surnameChanged,
            addressChanged
        };

        using PersonObserver = Observer<Person,StateChange>;

        explicit Person( std::string forename, std::string surname )
                : forename_{ std::move(forename) }
                , surname_{ std::move(surname) }
        {}

        bool attach( PersonObserver* observer )
        {
            auto [pos,success] = observers_.insert( observer );
            return success;
        }

        bool detach( PersonObserver* observer )
        {
            return ( observers_.erase( observer ) > 0U );
        }

        void notify( StateChange property )
        {
            for( auto iter=begin(observers_); iter!=end(observers_); )
            {
                auto const pos = iter++;
                (*pos)->update(*this,property);
            }
        }

        void forename(std::string newForename)
        {
            forename_ = std::move(newForename);
            notify( forenameChanged );
        }

        void surname(std::string newSurname )
        {
            surname_ = std::move(newSurname);
            notify( surnameChanged );
        }

        void address(std::string newAddress )
        {
            address_ = std::move(newAddress);
            notify( addressChanged );
        }

        [[nodiscard]]
        const std::string& forename() const {
            return forename_;
        }

        [[nodiscard]]
        const std::string& surname () const {
            return surname_;
        }

        [[nodiscard]]
        const std::string& address () const {
            return address_;
        }

    private:
        std::string forename_;
        std::string surname_;
        std::string address_;

        std::set<PersonObserver*> observers_;
    };

    void propertyChangedHandler(const Person& person,
                                Person::StateChange property)
    {
        if (property == Person::forenameChanged || property == Person::surnameChanged )
        {
            std::cout << "propertyChanged(): Forename --> " << person.forename() << std::endl;
        }
    }
}

void LambdaObserver_Test()
{
    using namespace LambdaObserver;
    using PersonObserver = Observer<Person,Person::StateChange>;

    PersonObserver nameObserver(propertyChangedHandler);

    PersonObserver addressObserver([](const Person & person, Person::StateChange property){
        if (Person::addressChanged == property)
        {
            std::cout << "addressObserver: Address --> " << person.address() << std::endl;
        }
    });

    Person homer( "Homer"     , "Simpson" );
    Person marge( "Marge"     , "Simpson" );
    Person monty( "Montgomery", "Burns"   );

    // Attaching observers
    homer.attach( &nameObserver );
    marge.attach( &addressObserver );
    monty.attach( &addressObserver );


    // Updating information on Homer Simpson
    homer.forename( "Homer Jay");

    // Updating information on Marge Simpson
    marge.address( "712 Red Bark Lane, Henderson, Clark County, Nevada 89011" );

    // Updating information on Montgomery Burns
    monty.address( "Springfield Nuclear Power Plant" );

    // Detaching observers
    homer.detach( &nameObserver );
}

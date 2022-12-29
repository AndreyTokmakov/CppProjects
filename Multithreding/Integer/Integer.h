#pragma once

#include <iostream>

class Integer {
protected:
    int value;

public:
    Integer();
    Integer(int val);
    Integer(const Integer &obj);

    virtual ~Integer();

    virtual int getValue() const;
    virtual void printInfo() const;
    virtual void setValue(int val);


public: /** Operators reload. **/
    friend std::ostream& operator<< (std::ostream& stream,
                                     const Integer& integer);

    friend Integer operator+(const Integer& left,
                             const Integer& right);

    friend Integer operator+=(Integer& left,
                              const Integer& right);

    friend bool operator==(const Integer& left,
                           const Integer& right);

    friend bool operator<(const Integer& left,
                          const Integer& right);

    friend bool operator>(const Integer& left,
                          const Integer& right);

    Integer operator=(const Integer& right) {
        if (this == &right) {
            return *this;
        }
        value = right.value;
        return *this;
    }

    friend Integer operator+(const Integer& integer);
    friend Integer operator-(const Integer& integer);
    friend Integer operator++(Integer& integer);
    friend Integer operator++(Integer& integer, int);
    friend Integer operator--(Integer& integer);
    friend Integer operator--(Integer& integer, int);
};
#include "Integer.h"

Integer::Integer() : value(0) {
    std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
}

Integer::Integer(int val) : value(val) {
    std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
}

Integer::Integer(const Integer &obj) {
    std::cout << __FUNCTION__ << " [Copy contructor]. (" << this->value << ")" << std::endl;
    this->value = obj.value;
}

Integer::~Integer() {
    std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
}

int Integer::getValue() const {
    return this->value;
}

void Integer::printInfo() const {
    std::cout << "Info : Integer value = " << this->value << std::endl;
}

void Integer::setValue(int val) {
    this->value = val;
}

std::ostream& operator<<(std::ostream& os, const Integer& integer) {
    os << integer.value;
    return os;
}

Integer operator+(const Integer& left, const Integer& right) {
    return Integer(left.value + right.value);
}

Integer operator+=(Integer& left, const Integer& right) {
    left.value += right.value;
    return left;
}

bool operator==(const Integer& left, const Integer& right) {
    return left.value == right.value;
}

bool operator<(const Integer& left, const Integer& right) {
    return left.value < right.value;
}

bool operator>(const Integer& left, const Integer& right) {
    return left.value > right.value;
}

Integer operator+(const Integer& integer) {
    return integer.value;
}

Integer operator-(const Integer& integer) {
    return Integer(-integer.value);
}

Integer operator++(Integer& integer) {
    integer.value++;
    return integer;
}

Integer operator++(Integer& integer, int) {
    Integer oldValue(integer.value);
    integer.value++;
    return oldValue;
}

Integer operator--(Integer& integer) {
    integer.value--;
    return integer;
}

Integer operator--(Integer& integer, int) {
    Integer oldValue(integer.value);
    integer.value--;
    return oldValue;
}

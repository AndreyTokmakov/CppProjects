//============================================================================
// Name        : main.cpp
// Created on  : 07.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <iostream>
#include <memory>
#include <math.h>
#include <algorithm>

#include <thread>
#include <future>

#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <string>
#include <string_view>
#include <mutex>
#include <atomic>
#include <numeric>
#include <fstream>

#include "matplotlibcpp.h"
#include <cmath>

namespace plt = matplotlibcpp;

void Test1() {
    int n = 5000;
    std::vector<double> x(n), y(n); // z(n), w(n,2);
    for(int i=0; i<n; ++i) {
        x.at(i) = i*i;
        y.at(i) = sin(2 * M_PI * i / 360.0);
        // z.at(i) = log(i);
    }

    plt::plot(x, y);
    plt::show();
}

void Test2() {
    int n = 5000;
    std::vector<double> x(n), y(n), y1(n);
    for(int i=0; i<n; ++i) {
        x.at(i) = i*i;
        y.at(i) = sin(2 * M_PI * i / 360.0);
        y1.at(i) = sin(3 * M_PI * i / 360.0);
        // z.at(i) = log(i);
    }

    // plt::subplot(2, 1, 0);

    plt::plot(x, y);
    plt::plot(x, y1,"r--");
    plt::show();
}

/*
void Test2() {
    // Prepare data.
    int n = 5000;
    std::vector<double> x(n), y(n), z(n), w(n,2);
    for(int i=0; i<n; ++i) {
        x.at(i) = i*i;
        y.at(i) = sin(2*M_PI*i/360.0);
        z.at(i) = log(i);
    }

    // Set the size of output image to 1200x780 pixels
    plt::figure_size(1200, 780);
    // Plot line from given x and y data. Color is selected automatically.
    plt::plot(x, y);
    // Plot a red dashed line from given x and y data.
    plt::plot(x, w,"r--");
    // Plot a line whose name will show up as "log(x)" in the legend.
    // plt::named_plot("log(x)", x, z);
    // Set x-axis to interval [0,1000000]
    plt::xlim(0, 1000*1000);
    // Add graph title
    plt::title("Sample figure");
    // Enable legend.
    plt::legend();
}

void Test3() {
    // Prepare data.
    int n = 5000; // number of data points
    std::vector<double> x(n),y(n);
    for(int i=0; i<n; ++i) {
        double t = 2*M_PI*i/n;
        x.at(i) = 16*sin(t)*sin(t)*sin(t);
        y.at(i) = 13*cos(t) - 5*cos(2*t) - 2*cos(3*t) - cos(4*t);
    }

    // plot() takes an arbitrary number of (x,y,format)-triples.
    // x must be iterable (that is, anything providing begin(x) and end(x)),
    // y must either be callable (providing operator() const) or iterable.
    plt::plot(x, y, "r-", x, [](double d) { return 12.5+abs(sin(d)); }, "k-");


    // show plots
    plt::show();
}
*/

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{

    // Test1();
    Test2();
    // Test3();

    return 0;
}
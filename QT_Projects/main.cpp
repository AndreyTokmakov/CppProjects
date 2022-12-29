//============================================================================
// Name        : QT_Projects.cpp
// Created on  : 
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : QT_Projects
//============================================================================

#include <optional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>
#include <cmath>
#include <exception>
#include <random>
#include <thread>
#include <future>
#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <vector>
#include <any>
#include <list>
#include <map>
#include <algorithm>
#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <span>


#include <QtCore>
#include <QWidget>
#include <QApplication>




int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    QApplication app(argc, argv);

    QWidget window;

    window.resize(250, 150);
    window.setWindowTitle("Simple example");
    window.show();

    return app.exec();


    return EXIT_SUCCESS;
}


/**============================================================================
Name        : OpenCV.cpp
Created on  : 10.01.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : OpenSSL C++ project
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>

#include <opencv2/opencv.hpp>

namespace Experiments
{
    void DisplayImage(std::string_view path)
    {
        cv::Mat image;
        image = imread( path.data(), cv::IMREAD_COLOR );
        if ( !image.data )
        {
            printf("No image data \n");
            return;
        }

        namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
        imshow("Display Image", image);
        cv::waitKey(0);
    }
}

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    Experiments::DisplayImage("/home/andtokm/Documents/FUN/cat.jpg");

    return EXIT_SUCCESS;
}


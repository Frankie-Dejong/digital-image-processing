#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <opencv2/opencv.hpp>
#include "Pixel.h"

class Image 
{
    int width;
    int height;       
    std::vector<Pixel> pixels;
    std::vector<uchar> grays;
    std::vector<uchar> reds;
    std::vector<uchar> greens;
    std::vector<uchar> blues;
    std::array<int, 256> gray_histogram = {};
    std::array<int, 256> red_histogram = {};
    std::array<int, 256> green_histogram = {};
    std::array<int, 256> blue_histogram = {};

    void calcGray();
    void calcRGB();
    void calcHistogramRGB();
    void calcGrayHistogram();
    void histogramEqualization();
    void histogramEqualizationRGB();
    void linearTransform(uchar c, uchar d);
    void logTransform(float alpha);
    void gammaCorrection(float gamma);
    void multiLinearTransform(std::array<unsigned char, 4> positions);
public:
    Image() = default;
    Image(std::string filename);
    void outputToPPM(std::string filename);
    void outputGrayToPPM(std::string filename);
    void outputRGBToPPM(std::string filename);
    void process(float gamma);
    void process(uchar c, uchar d, float gamma);
    void process(float alpha, float gamma);
    void process(std::array<unsigned char, 4> positions, float gamma);
    void process();
};

#endif // __IMAGE_H__
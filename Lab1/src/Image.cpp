#include "Image.h"
#include "iostream"
#include "fstream"

Image::Image() {
    this->height = 255;
    this->width = 255;
    this->pixels = std::vector<Pixel>(255*255, Pixel{255, 255, 255});
    calcGray();
}


Image::Image(const std::string filename) {
    cv::Mat mat;
    try {
        mat = cv::imread(filename);
    } catch(cv::Exception& e) {
        std::cerr << "Error: " << "Cannot open Image: " << filename <<  std::endl;
        exit(-1);
    }
    this->height = mat.rows;
    this->width = mat.cols;
    cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
    assert(mat.channels() == 3);
    for(int i = 0;i < height; i ++) {
        unsigned char *data = mat.ptr<unsigned char>(i);
        for(int j = 0; j < width * 3; j += 3) {
            this->pixels.push_back(Pixel{data[j], data[j+1], data[j+2]});
        }
    }
    calcGray();
}


void Image::outputToPPM(const std::string filename) {
    std::ofstream fout(filename);
    fout << "P3\n" << width << " " << height << "\n255\n";
    for(int i = 0; i < height; i++) 
        for(int j = 0; j < width; j++) 
            fout << int(pixels[i * width + j].r) << " " << int(pixels[i * width + j].g) << " " << int(pixels[i * width + j].b) << std::endl;
    fout.close();       
}

void Image::calcGray() {
    for(int i = 0;i < pixels.size(); i ++) {
        uchar gray = (uchar) ((int(pixels[i].r) * 38 + int(pixels[i].g) * 75 + int(pixels[i].b) * 15) >> 7);
        grays.push_back(gray);
    }
    calcGrayHistogram();
}

void Image::outputGrayToPPM(const std::string filename) {
    std::ofstream fout(filename);
    fout << "P3\n" << width << " " << height << "\n255\n";
    for(int i = 0; i < height; i++) 
        for(int j = 0; j < width; j++) 
            fout << int(grays[i * width + j]) << " " << int(grays[i * width + j]) << " " << int(grays[i * width + j]) << std::endl;
    fout.close();       
}


void Image::calcGrayHistogram() {
    gray_histogram.fill(0);
    for(uchar gray: grays) {
        gray_histogram[gray] ++;
    }
}


void Image::histogramEqualization() {
    std::vector<int> gray_freq;
    gray_freq.push_back(gray_histogram[0]);

    for(int i = 1;i < gray_histogram.size(); i ++) {
        gray_freq.push_back(gray_freq[i-1] + gray_histogram[i]);
    }

    for(int i = 0;i < grays.size(); i ++) {
        int gray = int(grays[i]);
        int converted = int(255.0 * gray_freq[gray] / (width * height) + 0.5);
        grays[i] = (uchar) converted;
    }
    calcGrayHistogram();
}


void Image::linearTransform(uchar c, uchar d) {
    for(int i = 0;i < grays.size(); i ++) {
        grays[i] = (uchar) (((float) d - (float) c) / 255 * grays[i] + c);
    }
}


void Image::logTransform(float alpha) {
    for(int i = 0;i < grays.size(); i ++) {
        grays[i] = (uchar) (alpha * log(1 + (float) grays[i]) * 32);
    }
}

void Image::gammaCorrection(float gamma) {
    for(int i = 0;i < grays.size(); i ++) {
        grays[i] = (uchar) (pow(grays[i] / 255.0, gamma) * 255.0);
    }
}

void Image::multiLinearTransform(std::array<uchar, 4> positions) {
    uchar x1 = positions[0];
    uchar y1 = positions[1];
    uchar x2 = positions[2];
    uchar y2 = positions[3];
    for(int i = 0;i < grays.size(); i ++) {
        if (grays[i] <= x1) {
            grays[i] = (uchar) ((float) grays[i] / (float) x1 * (float) y1);
        } else if (grays[i] >= x1 && grays[i] <= x2) {
            grays[i] = (uchar) ((float) (grays[i] - x1) / (float) (x2 - x1) * (float) (y2 - y1) + (float) y1);
        } else {
            grays[i] = (uchar) ((float) (255 - grays[i]) / (float) (255 - x2) * (float) (255 - y2) + (float) y2);
        }
    }
}



void Image::process(float gamma) {
    histogramEqualization();
    gammaCorrection(gamma);
}

void Image::process(uchar c, uchar d, float gamma) {
    histogramEqualization();
    linearTransform(c, d);
    gammaCorrection(gamma);
}

void Image::process(float alpha, float gamma) {
    histogramEqualization();
    logTransform(alpha);
    gammaCorrection(gamma);
}


void Image::process(std::array<uchar, 4> positions, float gamma) {
    histogramEqualization();
    multiLinearTransform(positions);
    gammaCorrection(gamma);
}
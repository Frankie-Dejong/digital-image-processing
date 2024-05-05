#include "Image.h"
#include "iostream"
#include "fstream"


Image::Image(const std::string filename) {
    cv::Mat mat;
    try {
        mat = cv::imread(filename);
    } catch(...) {
        std::cerr << "Error: " << "Cannot open Image: " << filename <<  std::endl;
        exit(-1);
    }
    height = mat.rows;
    width = mat.cols;
    cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
    assert(mat.channels() == 3);
    for(int i = 0;i < height; i ++) {
        unsigned char *data = mat.ptr<unsigned char>(i);
        for(int j = 0; j < width * 3; j += 3) {
            pixels.push_back(Pixel{data[j], data[j+1], data[j+2]});
        }
    }
    calcGray();
    calcRGB();
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

void Image::calcRGB() {
    for(int i = 0;i < pixels.size(); i ++) {
        uchar r = pixels[i].r;
        uchar g = pixels[i].g;
        uchar b = pixels[i].b;
        reds.push_back(r);
        greens.push_back(g);
        blues.push_back(b);
    }
    calcHistogramRGB();
}


void Image::outputGrayToPPM(const std::string filename) {
    std::ofstream fout(filename);
    fout << "P3\n" << width << " " << height << "\n255\n";
    for(int i = 0; i < height; i++) 
        for(int j = 0; j < width; j++) 
            fout << int(grays[i * width + j]) << " " << int(grays[i * width + j]) << " " << int(grays[i * width + j]) << std::endl;
    fout.close();       
}


void Image::outputRGBToPPM(const std::string filename) {
    std::ofstream fout(filename);
    fout << "P3\n" << width << " " << height << "\n255\n";
    for(int i = 0; i < height; i++) 
        for(int j = 0; j < width; j++) 
            fout << int(reds[i * width + j]) << " " << int(greens[i * width + j]) << " " << int(blues[i * width + j]) << std::endl;
    fout.close();       
}


void Image::calcGrayHistogram() {
    gray_histogram.fill(0);
    for(uchar gray: grays) {
        gray_histogram[gray] ++;
    }
}

void Image::calcHistogramRGB() {
    red_histogram.fill(0);
    green_histogram.fill(0);
    blue_histogram.fill(0);
    for(uchar r: reds) {
        red_histogram[r]++;
    }
    for(uchar g: greens) {
        green_histogram[g]++;
    }
    for(uchar b: blues) {
        blue_histogram[b]++;
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

void Image::histogramEqualizationRGB() {
    std::vector<int> red_freq;
    red_freq.push_back(red_histogram[0]);

    for(int i = 1;i < red_histogram.size(); i ++) {
        red_freq.push_back(red_freq[i-1] + red_histogram[i]);
    }

    std::vector<int> green_freq;
    green_freq.push_back(green_histogram[0]);

    for(int i = 1;i < green_histogram.size(); i ++) {
        green_freq.push_back(green_freq[i-1] + green_histogram[i]);
    }

    std::vector<int> blue_freq;
    blue_freq.push_back(blue_histogram[0]);

    for(int i = 1;i < blue_histogram.size(); i ++) {
        blue_freq.push_back(blue_freq[i-1] + blue_histogram[i]);
    }

    for(int i = 0;i < pixels.size(); i ++) {
        int r = int(reds[i]);
        int g = int(greens[i]);
        int b = int(blues[i]);
        int converted_r = int(255.0 * red_freq[r] / (width * height) + 0.5);
        int converted_g = int(255.0 * green_freq[g] / (width * height) + 0.5);
        int converted_b = int(255.0 * blue_freq[b] / (width * height) + 0.5);
        reds[i] = (uchar) converted_r;
        greens[i] = (uchar) converted_g;
        blues[i] = (uchar) converted_b;
    }
    calcHistogramRGB();
    calcGray();
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
            grays[i] = (uchar) ((float) (grays[i] - x2) / (float) (255 - x2) * (float) (255 - y2) + (float) y2);
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
    calcGrayHistogram();
}

void Image::process() {
    histogramEqualizationRGB();
}

void Image::fftTransform() {
    std::complex<double> A[height][width];
    std::complex<double> B[height][width];

    double pi = std::acos(-1);

    for (int i = 0;i < height; ++i) {
        for (int j = 0;j < width; ++j) {
            std::complex<double> Aij = 0;
            for (int k = 0;k < width; ++k) {
                double gray = (double) grays[i * width + k];
                double theta = -2 * pi * k * j / (double) width;
                Aij += gray * std::exp(std::complex<double>(0, theta));
            }
            A[i][j] = Aij;
        }
    }

    for (int j = 0;j < width; ++j) {
        for (int i = 0;i < height; ++i) {
            std::complex<double> Bij = 0;
            for (int k = 0;k < height; ++k) {
                auto Akj = A[k][j];
                double theta = -2 * pi * k * i / (double) height;
                Bij += Akj * std::exp(std::complex<double>(0, theta));
            }
            B[i][j] = Bij;
        }
    }


    double max = 0;
    for (int i = 0;i < height; ++i) 
        for (int j = 0;j < width; ++j) {
            double magnitude = 20 * std::log(std::abs(B[i][j]) + 1);
            max = std::max(max, magnitude);
        }
    for (int i = 0;i < height; ++i) {
        for (int j = 0;j < width; ++j) {
            double magnitude = 20 * std::log(std::abs(B[(i + height / 2) % height][(j + width / 2) % width]) + 1);
            grays[i * width + j] = (uchar) ((255 * magnitude) / max);
        }
    }
}

void Image::cosineTransform() {
    double A[height][width];
    double B[height][width];

    double pi = std::acos(-1);

    for (int i = 0;i < height; ++i) {
        for (int j = 0;j < width; ++j) {
            double Aij = 0;
            for (int k = 0;k < width; ++k) {
                double gray = (double) grays[i * width + k];
                double theta = (k + 0.5) * pi * j / (double) width;
                Aij += gray * std::cos(theta);
            }
            A[i][j] = Aij;
        }
    }

    for (int j = 0;j < width; ++j) {
        for (int i = 0;i < height; ++i) {
            double Bij = 0;
            for (int k = 0;k < height; ++k) {
                auto Akj = A[k][j];
                double theta = (k + 0.5) * pi * i / (double) height;
                Bij += Akj * std::cos(theta);
            }
            B[i][j] = Bij;
        }
    }


    double max = 0;
    for (int i = 0;i < height; ++i) 
        for (int j = 0;j < width; ++j) {
            double magnitude = 20 * std::log(std::abs(B[i][j]) + 1);
            max = std::max(max, magnitude);
        }
    for (int i = 0;i < height; ++i) {
        for (int j = 0;j < width; ++j) {
            double magnitude = 20 * std::log(std::abs(B[i][j]) + 1);
            grays[i * width + j] = (uchar) ((255 * magnitude) / max);
        }
    }
}
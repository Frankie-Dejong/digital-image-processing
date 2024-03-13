#include "ArgParser.h"
#include <iostream>

void help() {
    std::cout << "Usage:./main [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-h, --help: Display this help message" << std::endl;
    std::cout << "-i, --input: Input image file path" << std::endl;
    std::cout << "-o, --output: Output image file path" << std::endl;
    std::cout << "-rg, --raw_gray: Output raw grayscale image" << std::endl;
    std::cout << "-l, --linear: Apply linear transformation with parameters c-d" << std::endl;
    std::cout << "-lg, --log: Apply logarithmic transformation with parameters c";
    std::cout << "-g, --gamma: Apply gamma correction with parameter gamma" << std::endl;
    std::cout << "-ml, --multi_linear: Apply multiple linear transformation with parameters x1-y1-x2-y2" << std::endl;
}


ArgParser::ArgParser(int argc, char** argv) {
    if (argc == 1) {
        help();
        exit(-1);
    }
    for (int i = 1;i < argc;) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            help();
            exit(0);
        } else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input")) {
            i++;
            if (i < argc) {
                input_file_path = argv[i];
            } else {
                std::cout << "Error: Input file path not specified" << std::endl;
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) {
            i++;
            if (i < argc) {
                output_file_path = argv[i];
            } else {
                std::cout << "Error: Output file path not specified" << std::endl;
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-rg") || !strcmp(argv[i], "--raw_gray")) {
            i++;
            if (i < argc) {
                raw_gray_image = argv[i];
            } else {
                std::cout << "Error: Raw grayscale image path not specified" << std::endl;
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--linear")) {
            i++;
            int c_ = 0, d_ = 0, flag = 0;
            if (i < argc) {
                for(int j = 0; argv[i][j] != 0; j ++) {
                    if(argv[i][j] == '-') {
                        flag = 1;
                        continue;
                    }
                    if(!flag) {
                        c_ = 10 * c_ + (argv[i][j] - '0');
                    } else {
                        d_ = 10 * d_ + (argv[i][j] - '0');
                    }
                }
                c = (unsigned char) c_;
                d = (unsigned char) d_;
                if (!flag) {
                    std::cout << "Error: Linear transformation parameters should be in the form of c-d" << std::endl;
                    exit(-1);
                }
                need_linear_transform = true;
            } else {
                std::cout << "Error: Linear transformation parameters not specified" << std::endl;
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-lg") || !strcmp(argv[i], "--log")) {
            i++;
            if (i < argc) {
                alpha = std::stof(argv[i]);
                need_log_transform = true;
            } else {
                std::cout << "Error: Logarithmic transformation parameters not specified" << std::endl;
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-g") || !strcmp(argv[i], "--gamma")) {
            i++;
            if (i < argc) {
                gamma = std::stof(argv[i]);
            } else {
                std::cout << "Error: Gamma correction parameter not specified" << std::endl;
                exit(-1);
            }
        } else if (!strcmp(argv[i], "-ml") || !strcmp(argv[i], "--multi_linear")) {
            i++;
            int x1_ = 0, y1_ = 0, x2_ = 0, y2_ = 0, flag = 0;
            if (i < argc) {
                for(int j = 0; argv[i][j] != 0; j ++) { 
                    if(argv[i][j] == '-') {
                        flag += 1;
                        continue;
                    }
                    if(!flag) {
                        x1_ = 10 * x1_ + (argv[i][j] - '0');
                    } else if(flag == 1) {
                        y1_ = 10 * y1_ + (argv[i][j] - '0');
                    } else if(flag == 2) {
                        x2_ = 10 * x2_ + (argv[i][j] - '0');
                    } else {
                        y2_ = 10 * y2_ + (argv[i][j] - '0');
                    }
                }
                positions[0] = (unsigned char) x1_;
                positions[1] = (unsigned char) y1_;
                positions[2] = (unsigned char) x2_;
                positions[3] = (unsigned char) y2_;
                if (flag != 3) {
                    std::cout << "Error: Multiple linear transformation parameters should be in the form of x1-y1-x2-y2" << std::endl;
                    exit(-1);
                }
                need_multi_linear_transform = true;
            } else {
                std::cout << "Error: Multiple linear transformation parameters not specified" << std::endl;
                exit(-1);
            }
        }
        else {
            std::cout << "Error: Invalid option " << argv[i] << std::endl;
            exit(-1);
        }
        i++;
    }
}

std::string ArgParser::getInputFilePath() const {
    return input_file_path;
}


std::string ArgParser::getOutputFilePath() const {
    return output_file_path;
}


std::string ArgParser::getRawGrayImagePath() const {
    return raw_gray_image;
}

unsigned char ArgParser::getC() const {
    return c;
}

unsigned char ArgParser::getD() const {
    return d;
}

bool ArgParser::needLinearTransform() const {
    return need_linear_transform;
}

bool ArgParser::needLogTransform() const {
    return need_log_transform;
}

float ArgParser::getGamma() const {
    return gamma;
}

float ArgParser::getAlpha() const {
    return alpha;
}

std::array<unsigned char, 4> ArgParser::getPositions() const {
    return positions;
}

bool ArgParser::needMultiLinearTransform() const {
    return need_multi_linear_transform;
}


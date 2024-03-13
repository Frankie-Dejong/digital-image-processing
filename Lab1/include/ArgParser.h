#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <array>
#include <string>

class ArgParser {
    float gamma = 1.0f;
    float alpha = 1.0f;
    unsigned char c, d = 0;
    std::array<unsigned char, 4> positions = {};
    std::string input_file_path;
    std::string output_file_path;
    std::string raw_gray_image;
    bool need_linear_transform = false;
    bool need_log_transform = false;
    bool need_multi_linear_transform = false;
public:
    ArgParser(int argc, char** argv);
    std::string getInputFilePath() const;
    std::string getOutputFilePath() const;
    std::string getRawGrayImagePath() const;
    std::array<unsigned char, 4> getPositions() const;
    float getGamma() const;
    float getAlpha() const;
    unsigned char getC() const;
    unsigned char getD() const;
    bool needLinearTransform() const;
    bool needLogTransform() const;
    bool needMultiLinearTransform() const;
};


#endif // ARGPARSER_H
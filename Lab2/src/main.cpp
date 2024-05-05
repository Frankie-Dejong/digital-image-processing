#include <iostream>
#include <filesystem>
#include "Image.h"
#include "ArgParser.h"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    ArgParser argParser(argc, argv);

    Image img(argParser.getInputFilePath());

    if (!argParser.getRawGrayImagePath().empty()) {
        img.outputGrayToPPM(argParser.getRawGrayImagePath());
    }

    if(argParser.needLinearTransform()) 
        img.process(argParser.getC(), argParser.getD(), argParser.getGamma());
    else if(argParser.needLogTransform()) 
        img.process(argParser.getAlpha(), argParser.getGamma());
    else if (argParser.needMultiLinearTransform()) 
        img.process(argParser.getPositions(), argParser.getGamma());
    else if (argParser.needFFTTransform()) 
        img.fftTransform();  
    else if (argParser.needCosTransform())
        img.cosineTransform();
    else 
        img.process(argParser.getGamma());
    

    img.outputGrayToPPM(argParser.getOutputFilePath());
    return 0;
}
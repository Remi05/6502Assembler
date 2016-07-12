//Author: Rémi Pelletier
//File:   main.cpp
//Desc.:  Entry point of the program.

//TODO: Add license boiler plate.

#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "Mos6502Assembler.h"


static const std::string DEFAULT_OUTPUT_FILE_NAME("output.bin");

int main(int argc, char* argv[])
{
    //Exits if no file is specified.
    if (argc <= 1) {
        std::cerr << "Error : No file specified." << std::endl;
        return EXIT_FAILURE;
    }

    std::string inputFileName = argv[1];
    std::ifstream input(inputFileName);

    //Exits if the file can't be opened.
    if (input.fail()) {
        std::cerr << "An error occured when opening " << inputFileName << "." << std::endl;
        input.close();
        return EXIT_FAILURE;
    }

    //Tries reading the file and exits if an errors occurs.
    std::stringstream inputBuffer;
    try {
        inputBuffer << input.rdbuf();
    }
    catch (std::exception e) {
        std::cerr << "File could not be read correctly. Error message : " << e.what() << std::endl;
        input.close();
        return EXIT_FAILURE;
    }

    input.close();

    //Uses the given output file name if one is given, 
    //otherwise uses the default file name.
    std::string outputFileName = DEFAULT_OUTPUT_FILE_NAME;
    if (argc >= 3) {
        outputFileName = argv[2];
    }

    std::ofstream output(outputFileName, std::ios::binary);

    //Tries compiling the source code file and exits if the compilation fails.
    try {
        Mos6502Assembler::compile(inputBuffer, output);
    }
    catch (std::exception e) {
        std::cerr << "Compilation failed. Error message : " << e.what()
            << std::endl;
        output.close();
        return EXIT_FAILURE;
    }

    output.close();

    std::clog << "Compilation completed successfully." << std::endl;

    return EXIT_SUCCESS;
}
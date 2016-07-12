//Author: Rémi Pelletier
//File:   Mos6502Compiler.h
//Desc.:  Header file containing the declaration of the neccesarry
//        elements to use of the compiler.

#pragma once
#include <iostream>

namespace Mos6502Compiler {

    class InvalidInstructionException : std::invalid_argument
    {
    public:
        InvalidInstructionException();
    };

    class InvalidAddressingModeException : std::invalid_argument
    {
    public:
        InvalidAddressingModeException();
    };

    class MissingArgumentException : std::invalid_argument
    {
    public:
        MissingArgumentException();
    };


    void compile(std::istream& input, std::ostream& output);
}
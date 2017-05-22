//Author: Rémi Pelletier
//File:   Mos6502Assembler.cpp
//Desc.:  Source file containing the compilation logic.

//TODO: Add license boiler plate.
//TODO: Review error handling.

#include <cstdint>
#include <exception>
#include <iostream>
#include <map>
#include <regex>
#include <stdexcept>
#include <string>
#include "Mos6502Assembler.h"

namespace Mos6502Assembler
{
	InvalidInstructionException::InvalidInstructionException()
		:std::invalid_argument("The given instruction is not valid.")
	{ }

	InvalidAddressingModeException::InvalidAddressingModeException()
		: std::invalid_argument("This addressing mode is not a valid addressing mode for the given instruction.")
	{ }

	MissingArgumentException::MissingArgumentException()
		: std::invalid_argument("Expecting argument, none found.")
	{ }


	//An emuration of every valid addressing mode for the MOS Technology 6502
	//instruction set, plus an entry to represent an invalid addressing mode
	//when parsing. The entries are given values that are powers of 2
	//in order to facilitate and speed up the process of evaluating wether
	//the given addressing mode has an 8-bit argument, a 16-bit argument,
	//or no argument at all.
	enum AddressingMode
	{
		ACCUMULATOR = 0x0001,
		ABSOLUTE = 0x0002,
		ABSOLUTE_X = 0x0004,
		ABSOLUTE_Y = 0x0008,
		IMMEDIATE = 0x0010,
		IMPLIED = 0x0020,
		INDIRECT = 0x0040,
		INDEXED_INDIRECT_X = 0x0080,
		INDIRECT_INDEXED_Y = 0x0100,
		RELATIVE = 0x0200,
		ZEROPAGE = 0x0400,
		ZEROPAGE_X = 0x0800,
		ZEROPAGE_Y = 0x1000,
		INVALID_ADDRESSING_MODE = 0x2000
	};

	//Represents a MOS Technology 6502 instruction.
	//The dictionnary contains every valid addressing mode
	//for the given instruction along with the corresponding
	//opcode, given as <key: AddressingMode, value: opcode> pairs.
	struct Mos6502Instruction
	{
		std::map<AddressingMode, uint8_t> opcodes;
	};


	//A dictionnary containg all the MOS Technology instructions.
	//The key of each instruction is the uppercase string of said instruction.
	static const std::map<std::string, Mos6502Instruction> MOS6502_INSTRUCTIONS =
	{
		{ "ADC",{ { { IMMEDIATE, 0x69 },{ ZEROPAGE, 0x65 },{ ZEROPAGE_X, 0x75 },{ ABSOLUTE, 0x6D },{ ABSOLUTE_X, 0x7D },{ ABSOLUTE_Y, 0x79 },{ INDEXED_INDIRECT_X, 0x61 },{ INDIRECT_INDEXED_Y, 0x71 } } } },
		{ "AND",{ { { IMMEDIATE, 0x29 },{ ZEROPAGE, 0x25 },{ ZEROPAGE_X, 0x35 },{ ABSOLUTE, 0x2D },{ ABSOLUTE_X, 0x3D },{ ABSOLUTE_Y, 0x39 },{ INDEXED_INDIRECT_X, 0x21 },{ INDIRECT_INDEXED_Y, 0x31 } } } },
		{ "ASL",{ { { ACCUMULATOR, 0x0A },{ ZEROPAGE, 0x06 },{ ZEROPAGE_X, 0x16 },{ ABSOLUTE, 0x0E },{ ABSOLUTE_X, 0x1E } } } },
		{ "BCC",{ { { RELATIVE, 0x90 } } } },
		{ "BCS",{ { { RELATIVE, 0xB0 } } } },
		{ "BEQ",{ { { RELATIVE, 0xF0 } } } },
		{ "BIT",{ { { ZEROPAGE, 0x24 },{ ABSOLUTE, 0x2C } } } },
		{ "BMI",{ { { RELATIVE, 0x30 } } } },
		{ "BNE",{ { { RELATIVE, 0xD0 } } } },
		{ "BPL",{ { { RELATIVE, 0x10 } } } },
		{ "BRK",{ { { IMPLIED, 0x00 } } } },
		{ "BVC",{ { { RELATIVE, 0x50 } } } },
		{ "BVS",{ { { RELATIVE, 0x70 } } } },
		{ "CLC",{ { { IMPLIED, 0x18 } } } },
		{ "CLD",{ { { IMPLIED, 0xD8 } } } },
		{ "CLI",{ { { IMPLIED, 0x58 } } } },
		{ "CLV",{ { { IMPLIED, 0xB8 } } } },
		{ "CMP",{ { { IMMEDIATE, 0xC9 },{ ZEROPAGE, 0xC5 },{ ZEROPAGE_X, 0xD5 },{ ABSOLUTE, 0xCD },{ ABSOLUTE_X, 0xDD },{ ABSOLUTE_Y, 0xD9 },{ INDEXED_INDIRECT_X, 0xC1 },{ INDIRECT_INDEXED_Y, 0xD1 } } } },
		{ "CPX",{ { { IMMEDIATE, 0xE0 },{ ZEROPAGE, 0xE4 },{ ABSOLUTE, 0xEC } } } },
		{ "CPY",{ { { IMMEDIATE, 0xC0 },{ ZEROPAGE, 0xC4 },{ ABSOLUTE, 0xCC } } } },
		{ "DEC",{ { { ZEROPAGE, 0xC6 },{ ZEROPAGE_X, 0xD6 },{ ABSOLUTE, 0xCE },{ ABSOLUTE_X, 0xDE } } } },
		{ "DEX",{ { { IMPLIED, 0xCA } } } },
		{ "DEY",{ { { IMPLIED, 0x88 } } } },
		{ "EOR",{ { { IMMEDIATE, 0x49 },{ ZEROPAGE, 0x45 },{ ZEROPAGE_X, 0x55 },{ ABSOLUTE, 0x4D },{ ABSOLUTE_X, 0x5D },{ ABSOLUTE_Y, 0x59 },{ INDEXED_INDIRECT_X, 0x41 },{ INDIRECT_INDEXED_Y, 0x51 } } } },
		{ "INC",{ { { ZEROPAGE, 0xE6 },{ ZEROPAGE_X, 0xF6 },{ ABSOLUTE, 0xEE },{ ABSOLUTE_X, 0xFE } } } },
		{ "INX",{ { { IMPLIED, 0xE8 } } } },
		{ "INY",{ { { IMPLIED, 0xC8 } } } },
		{ "JMP",{ { { ABSOLUTE, 0x4C },{ INDIRECT, 0x6C } } } },
		{ "JSR",{ { { ABSOLUTE, 0x20 } } } },
		{ "LDA",{ { { IMMEDIATE, 0xA9 },{ ZEROPAGE, 0xA5 },{ ZEROPAGE_X, 0xB5 },{ ABSOLUTE, 0xAD },{ ABSOLUTE_X, 0xBD },{ ABSOLUTE_Y, 0xB9 },{ INDEXED_INDIRECT_X, 0xA1 },{ INDIRECT_INDEXED_Y, 0xB1 } } } },
		{ "LDX",{ { { IMMEDIATE, 0xA2 },{ ZEROPAGE, 0xA6 },{ ZEROPAGE_Y, 0xB6 },{ ABSOLUTE, 0xAE },{ ABSOLUTE_Y, 0xBE } } } },
		{ "LDY",{ { { IMMEDIATE, 0xA0 },{ ZEROPAGE, 0xA4 },{ ZEROPAGE_X, 0xB4 },{ ABSOLUTE, 0xAC },{ ABSOLUTE_X, 0xBC } } } },
		{ "LSR",{ { { ACCUMULATOR, 0x4A },{ ZEROPAGE, 0x46 },{ ZEROPAGE_X, 0x56 },{ ABSOLUTE, 0x4E },{ ABSOLUTE_X, 0x5E } } } },
		{ "NOP",{ { { IMPLIED, 0xEA } } } },
		{ "ORA",{ { { IMMEDIATE, 0x09 },{ ZEROPAGE, 0x05 },{ ZEROPAGE_X, 0x15 },{ ABSOLUTE, 0x0D },{ ABSOLUTE_X, 0x1D },{ ABSOLUTE_Y, 0x19 },{ INDEXED_INDIRECT_X, 0x01 },{ INDIRECT_INDEXED_Y, 0x11 } } } },
		{ "PHA",{ { { IMPLIED, 0x48 } } } },
		{ "PHP",{ { { IMPLIED, 0x08 } } } },
		{ "PLA",{ { { IMPLIED, 0x68 } } } },
		{ "PLP",{ { { IMPLIED, 0x28 } } } },
		{ "ROL",{ { { ACCUMULATOR, 0x2A },{ ZEROPAGE, 0x26 },{ ZEROPAGE_X, 0x36 },{ ABSOLUTE, 0x2E },{ ABSOLUTE_X, 0x3E } } } },
		{ "ROR",{ { { ACCUMULATOR, 0x6A },{ ZEROPAGE, 0x66 },{ ZEROPAGE_X, 0x76 },{ ABSOLUTE, 0x6E },{ ABSOLUTE_X, 0x7E } } } },
		{ "RTI",{ { { IMPLIED, 0x40 } } } },
		{ "RTS",{ { { IMPLIED, 0x60 } } } },
		{ "SBC",{ { { IMMEDIATE, 0xE9 },{ ZEROPAGE, 0xE5 },{ ZEROPAGE_X, 0xF5 },{ ABSOLUTE, 0xED },{ ABSOLUTE_X, 0xFD },{ ABSOLUTE_Y, 0xF9 },{ INDEXED_INDIRECT_X, 0xE1 },{ INDIRECT_INDEXED_Y, 0xF1 } } } },
		{ "SEC",{ { { IMPLIED, 0x38 } } } },
		{ "SED",{ { { IMPLIED, 0xF8 } } } },
		{ "SEI",{ { { IMPLIED, 0x78 } } } },
		{ "STA",{ { { ZEROPAGE, 0x85 },{ ZEROPAGE_X, 0x95 },{ ABSOLUTE, 0x8D },{ ABSOLUTE_X, 0x9D },{ ABSOLUTE_Y, 0x99 },{ INDEXED_INDIRECT_X, 0x81 },{ INDIRECT_INDEXED_Y, 0x91 } } } },
		{ "STX",{ { { ZEROPAGE, 0x86 },{ ZEROPAGE_Y, 0x96 },{ ABSOLUTE, 0x8E } } } },
		{ "STY",{ { { ZEROPAGE, 0x84 },{ ZEROPAGE_X, 0x94 },{ ABSOLUTE, 0x8C } } } },
		{ "TAX",{ { { IMPLIED, 0xAA } } } },
		{ "TAY",{ { { IMPLIED, 0xA8 } } } },
		{ "TSX",{ { { IMPLIED, 0xBA } } } },
		{ "TXA",{ { { IMPLIED, 0x8A } } } },
		{ "TXS",{ { { IMPLIED, 0x9A } } } },
		{ "TYA",{ { { IMPLIED, 0x98 } } } },
	};


	//Mask used to quickly determine if the given addressing mode
	//has an 8-bit argument.
	static const uint16_t ADDR_MODE_ARG_8BIT = AddressingMode::IMMEDIATE |
		AddressingMode::RELATIVE |
		AddressingMode::ZEROPAGE |
		AddressingMode::ZEROPAGE_X |
		AddressingMode::ZEROPAGE_Y |
		AddressingMode::INDEXED_INDIRECT_X |
		AddressingMode::INDIRECT_INDEXED_Y;

	//Mask used to quickly determine if the given addressing mode
	//has a 16-bit argument.
	static const uint16_t ADDR_MODE_ARG_16BIT = AddressingMode::INDIRECT |
		AddressingMode::ABSOLUTE |
		AddressingMode::ABSOLUTE_X |
		AddressingMode::ABSOLUTE_Y;


	//Regular expression used to match the instruction in a line.
	static const std::regex INSTRUCTION_REGEX("([A-Z]{3})",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	//Regular expression used to match the argument in a line.
	static const std::regex ARGUMENT_REGEX("\\$([A-F0-9]{2,4})",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	//Regular expressions used to determine the addressing mode of a given line.
	static const std::regex ABS_REGEX("\\$[A-F0-9]{4}",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	static const std::regex ABSX_REGEX("\\$[A-F0-9]{4}\\s*,\\s*X",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	static const std::regex ABSY_REGEX("\\$[A-F0-9]{4}\\s*,\\s*Y",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	static const std::regex IMM_REGEX("#\\$[A-F0-9]{2}",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	static const std::regex IND_REGEX("\\(\\s*\\$[A-F0-9]{4}\\s*\\)",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	static const std::regex INDX_REGEX("\\(\\s*\\$[A-F0-9]{2}\\s*,\\s*X\\s*\\)",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	static const std::regex INDY_REGEX("\\(\\s*\\$[A-F0-9]{2}\\s*\\)\\s*,\\s*Y",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	static const std::regex ZP_REL_REGEX("\\$[A-F0-9]{2}",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	static const std::regex ZPX_REGEX("\\$[A-F0-9]{2}\\s*,\\s*X",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);

	static const std::regex ZPY_REGEX("\\$[A-F0-9]{2}\\s*,\\s*Y",
		std::regex::ECMAScript |
		std::regex::icase |
		std::regex::optimize);



	//Determines the addressing mode used in a given line of code.
	//
	//Parameters: line        -> The line of code to be evaluated.
	//            instruction -> The instruction parsed from the given line.
	//
	//Return value: The addressing mode used in the given line.
	//              INVALID_ADDRESSING_MODE is returned if no valid addressing
	//              mode can be determined.
	AddressingMode determineAddressingMode(const std::string& line, const Mos6502Instruction& instruction)
	{
		if (std::regex_search(line, ABSX_REGEX)) {
			return AddressingMode::ABSOLUTE_X;
		}

		if (std::regex_search(line, ABSY_REGEX)) {
			return AddressingMode::ABSOLUTE_Y;
		}

		if (std::regex_search(line, ABS_REGEX)) {
			return AddressingMode::ABSOLUTE;
		}

		if (std::regex_search(line, IMM_REGEX)) {
			return AddressingMode::IMMEDIATE;
		}

		if (std::regex_search(line, IND_REGEX)) {
			return AddressingMode::INDIRECT;
		}

		if (std::regex_search(line, INDX_REGEX)) {
			return AddressingMode::INDEXED_INDIRECT_X;
		}

		if (std::regex_search(line, INDY_REGEX)) {
			return AddressingMode::INDIRECT_INDEXED_Y;
		}

		if (std::regex_search(line, ZPX_REGEX)) {
			return AddressingMode::ZEROPAGE_X;
		}

		if (std::regex_search(line, ZPY_REGEX)) {
			return AddressingMode::ZEROPAGE_Y;
		}

		if (std::regex_search(line, ZP_REL_REGEX)) {
			return instruction.opcodes.find(AddressingMode::ZEROPAGE) != instruction.opcodes.end() ?
				AddressingMode::ZEROPAGE : AddressingMode::RELATIVE;
		}

		if (instruction.opcodes.find(AddressingMode::IMPLIED) != instruction.opcodes.end()) {
			return AddressingMode::IMPLIED;
		}
		else if (instruction.opcodes.find(AddressingMode::ACCUMULATOR) != instruction.opcodes.end()) {
			return AddressingMode::ACCUMULATOR;
		}

		return AddressingMode::INVALID_ADDRESSING_MODE;
	}


	//Parses the argument from the given line.
	//
	//Parameters: line -> The line of code to be evaluated.
	//
	//Return value: The value of the parsed argument cast to the given type.
	//
	//Throws MissingArgumentException if no argument can be parsed from the
	//given line.
	template<class T>
	T parseArgument(const std::string& line)
	{
		std::smatch arg;
		if (!std::regex_search(line, arg, ARGUMENT_REGEX)) {
			throw MissingArgumentException();
		}
		return (T)std::stoul(arg.str(1), nullptr, 16);
	}


	//Compiles the MOS Technology 6502 instructions from the given input stream
	//and writes the obtained machine code to the output stream.
	//
	//Parameters: input  -> The input stream from which to read the instructions.
	//            output -> The output stream to which the machine code is written.
	//
	//Return value: None
	//
	//Throws InvalidInstructionException if an invalid instruction is read from the
	//input stream or if no instruction can be found in a non white-space line.
	//
	//Throws InvalisAddressingModeException if the addressing mode used in a line
	//doesn't correspond to a valid addressing mode for the given instruction.
	void compile(std::istream& input, std::ostream& output)
	{
		std::string line;
		while (std::getline(input, line)) {
			//Skips the current line if it's empty or consists only of white-space characters.
			if (line.length() == 0 || std::all_of(line.begin(), line.end(), isspace)) {
				continue;
			}

			//Replaces lower case letters for upper case ones.
			std::transform(line.begin(), line.end(), line.begin(), toupper);

			std::smatch matches;

			//Finds the instruction in the current line or raises an exception
			//if none is found.
			if (!std::regex_search(line, matches, INSTRUCTION_REGEX)) {
				throw InvalidInstructionException();
			}

			std::string instructionStr = matches.str(1);

			//Raises an exception if the given instruction is not a valid
			//MOS Technology 6502 instruction.
			if (MOS6502_INSTRUCTIONS.find(instructionStr) == MOS6502_INSTRUCTIONS.end()) {
				throw InvalidInstructionException();
			}

			Mos6502Instruction instruction = MOS6502_INSTRUCTIONS.at(instructionStr);

			//Determines the addressing mode based on the line's format.
			AddressingMode addressingMode = determineAddressingMode(line, instruction);

			//Raises an exception if the format of the line doesn't correspond to
			//any addressing mode or if the given addressing mode can't be used
			//with current instruction.
			if (addressingMode == AddressingMode::INVALID_ADDRESSING_MODE ||
				instruction.opcodes.find(addressingMode) == instruction.opcodes.end()) {
				throw InvalidAddressingModeException();
			}

			//Writes the opcode to the output stream.
			uint8_t opcode = instruction.opcodes[addressingMode];
			output.write((char*)&opcode, sizeof(opcode));

			//Parses the argument (if there is one) and
			//writes it to the output stream.
			if ((addressingMode & ADDR_MODE_ARG_8BIT) != 0) {
				uint8_t argVal = parseArgument<uint8_t>(line);
				output.write((char*)&argVal, sizeof(argVal));
			}
			else if ((addressingMode & ADDR_MODE_ARG_16BIT) != 0) {
				uint16_t argVal = parseArgument<uint16_t>(line);
				output.write((char*)&argVal, sizeof(argVal));
			}
		}
	}
}





#pragma region Dependencies

#include <bit>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "legv8_decoder.hpp"

#pragma endregion

int main() {
#pragma region Read instructions into memory
	std::vector<uint32_t> instructions;

	std::ifstream file("program.bin", std::ios::binary);
	if (!file)
		return false;

	file.seekg(0, std::ios::end);
	const auto streamEnd = file.tellg();
	if (streamEnd < 0 || streamEnd % 4 != 0)
		return false;

	const size_t length = static_cast<size_t>(streamEnd) >> 2;
	instructions.resize(length);

	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(instructions.data()), streamEnd);

	if (!(file.good() || file.eof() && file.gcount() == streamEnd)) {
		std::cerr << "Failed to read program.bin\n";
		return 1;
	}

	file.close();
#pragma endregion

#pragma region Identify which instructions are branches and where they point to
	constexpr size_t bitCount = sizeof(uintmax_t) * 8;
	constexpr size_t bitShift = std::bit_width(bitCount - 1);

	std::vector<uintmax_t> bitset;
	bitset.resize((length + bitCount - 1) >> bitShift);

	for (size_t programCounter = 0; programCounter < length; ++programCounter) {
		const uint32_t instruction = instructions[programCounter];
		const auto immediate = LEGv8::decode_branch(instruction);
		if (immediate == std::numeric_limits<int64_t>::min())
			continue;

		const int64_t target = static_cast<int64_t>(programCounter << 2) + immediate;
		if (target < 0 || target >= static_cast<int64_t>(length << 2) || target & 3)
			continue;

		const size_t index = target >> 2;
		bitset[index >> bitShift] |= uintmax_t(1) << (index & (bitCount - 1));
	}
#pragma endregion

#pragma region Iterate and concatenate in output
	size_t index = 0;
	size_t wordIndex = 0;

	uintmax_t pending = bitset.empty() ? 0 : bitset[0];

	while (index < length) {
		while (!pending && ++wordIndex < bitset.size())
			pending = bitset[wordIndex];

		size_t nextLabelIndex = length;

		if (pending) {
			nextLabelIndex = (wordIndex << bitShift) + std::countr_zero(pending);
			pending &= pending - 1;
		}

		for (; index < nextLabelIndex; ++index) {
			const uint32_t instruction = instructions[index];
			// output legv8 instruction string
		}

		if (nextLabelIndex == length)
			break;

		std::cout << 'L' << nextLabelIndex << ":\n";

		const uint32_t instruction = instructions[index];
		// output legv8 instruction string

		++index;
	}
#pragma endregion

	return 0;
}

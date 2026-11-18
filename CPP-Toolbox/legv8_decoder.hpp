#pragma once

#include <cstdint>
#include <limits>

namespace LEGv8 {

	enum class Format : uint8_t {

		UNKNOWN,
		R,
		I,
		D,
		B,
		CB,
		IM,
		IW

	};

	struct DecodedInstruction {

		Format format = Format::UNKNOWN;

		//uint8_t opcode = 0;

		uint8_t condition = 0;
		uint8_t rd = 0;
		uint8_t rn = 0;
		uint8_t rm = 0;
		uint8_t rt = 0;

		uint16_t shamt = 0;

		int64_t immediate = 0;

	};

	constexpr int64_t sign_extend(uint32_t value, unsigned bits) noexcept {
		const uint32_t sign = uint32_t(1) << (bits - 1);
		return static_cast<int64_t>(value ^ sign) - static_cast<int64_t>(sign);
	}

	constexpr int64_t decode_branch(uint32_t instruction) noexcept {
		const uint32_t op6 = instruction >> 26;
		const uint32_t op8 = instruction >> 24;

		// B (000101) and BL (100101) - 26-bit immediate
		if (op6 == 0b000101 || op6 == 0b100101)
			return sign_extend(instruction & 0x03FFFFFF, 26) * 4;

		// B.cond (01010100, bit 4 == 0), CBZ (10110100), CBNZ (10110101) - 19-bit immediate
		if ((op8 == 0b01010100 && (instruction & 0x10) == 0) || op8 == 0b10110100 || op8 == 0b10110101)
			return sign_extend((instruction >> 5) & 0x7FFFF, 19) * 4;

		return std::numeric_limits<int64_t>::min();
	}

	constexpr DecodedInstruction decode(uint32_t instruction) noexcept {
		DecodedInstruction result{};

		if ((instruction >> 21 & 0x7FF) == 0x458) {
			result.format = Format::R;
			result.rm = (instruction >> 16) & 0x1F;
			result.shamt = (instruction >> 10) & 0x3F;
			result.rn = (instruction >> 5) & 0x1F;
			result.rd = instruction & 0x1F;
			return result;
		}

		if ((instruction >> 22 & 0x3FF) == 0x244) {
			result.format = Format::I;
			result.immediate = sign_extend((instruction >> 10) & 0xFFF, 12);
			result.rn = (instruction >> 5) & 0x1F;
			result.rd = instruction & 0x1F;
			return result;
		}

		if ((instruction >> 21 & 0x7FF) == 0x7C2) {
			result.format = Format::D;
			result.immediate = sign_extend((instruction >> 12) & 0x1FF, 9);
			result.rn = (instruction >> 5) & 0x1F;
			result.rt = instruction & 0x1F;
			return result;
		}

		if ((instruction >> 26) == 0b000101) {
			result.format = Format::B;
			result.immediate = sign_extend(instruction & 0x03FFFFFF, 26) << 2;
			return result;
		}

		if ((instruction >> 24) == 0b01010100) {
			result.format = Format::CB;
			result.immediate = sign_extend((instruction >> 5) & 0x7FFFF, 19) << 2;
			result.rt = instruction & 0x1F;
			result.condition = instruction & 0xF;
			return result;
		}

		return result;
	}

}
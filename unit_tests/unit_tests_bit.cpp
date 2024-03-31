#include "unit_tests.h"

#if defined(CRSTL_UNIT_MODULES)
import crstl;
#else
#include "crstl/bit.h"
#endif

#if CRSTL_CPPVERSION >= CRSTL_CPP20
#include <bit>
#endif

void RunUnitTestsBit()
{
	// firstbitlow

	int bitop_fbl0 = crstl::firstbitlow(0);
	crstl_check(bitop_fbl0 == -1);

	int bitop_fbl1 = crstl::firstbitlow(1);
	crstl_check(bitop_fbl1 == 0);

	int bitop_fbl2 = crstl::firstbitlow(2u);
	crstl_check(bitop_fbl2 == 1);

	int bitop_fbl3 = crstl::firstbitlow(3u);
	crstl_check(bitop_fbl3 == 0);

	int bitop_fbl4 = crstl::firstbitlow((uint64_t)0xffff000000000000);
	crstl_check(bitop_fbl4 == 48);

	// firstbithigh

	int bitop_fbh0 = crstl::firstbithigh(0);
	crstl_check(bitop_fbh0 == -1);

	int bitop_fbh1 = crstl::firstbithigh(0xffffffffu);
	crstl_check(bitop_fbh1 == 31);

	int bitop_fbh2 = crstl::firstbithigh(1);
	crstl_check(bitop_fbh2 == 0);

	int bitop_fbh3 = crstl::firstbithigh(3);
	crstl_check(bitop_fbh3 == 1);

	// countr_zero

	int bitop_crz0 = crstl::countr_zero(0u);
	crstl_check(bitop_crz0 == 32);

	int bitop_crz1 = crstl::countr_zero(0xffffffffu);
	crstl_check(bitop_crz1 == 0);

	int bitop_crz2 = crstl::countr_zero(1u);
	crstl_check(bitop_crz2 == 0);

	int bitop_crz3 = crstl::countr_zero(3u);
	crstl_check(bitop_crz3 == 0);

	int bitop_crz4 = crstl::countr_zero(0xf0000000u);
	crstl_check(bitop_crz4 == 28);

	int bitop_crz5 = crstl::countr_zero(0xf0f0f0f0u);
	crstl_check(bitop_crz5 == 4);

	int bitop_crz6 = crstl::countr_zero((char)0);
	crstl_check(bitop_crz6 == 8);

	int bitop_crz7 = crstl::countr_zero((unsigned char)0xff);
	crstl_check(bitop_crz7 == 0);

	// countl_zero

	int bitop_clz0 = crstl::countl_zero(0u);
	crstl_check(bitop_clz0 == 32);

	int bitop_clz1 = crstl::countl_zero(0xffffffffu);
	crstl_check(bitop_clz1 == 0);

	int bitop_clz2 = crstl::countl_zero(1u);
	crstl_check(bitop_clz2 == 31);

	int bitop_clz3 = crstl::countl_zero(3u);
	crstl_check(bitop_clz3 == 30);

	int bitop_clz4 = crstl::countl_zero(0x0000000fu);
	crstl_check(bitop_clz4 == 28);

	int bitop_clz5 = crstl::countl_zero(0x0f0f0f0fu);
	crstl_check(bitop_clz5 == 4);

	int bitop_clz6 = crstl::countl_zero((unsigned char)1u);
	crstl_check(bitop_clz6 == 7);

	// popcnt

	int bitop_pcnt0 = crstl::popcount((uint16_t)0u);
	crstl_check(bitop_pcnt0 == 0);

	int bitop_pcnt1 = crstl::popcount((uint32_t)0u);
	crstl_check(bitop_pcnt1 == 0);

	int bitop_pcnt2 = crstl::popcount((uint64_t)0u);
	crstl_check(bitop_pcnt2 == 0);

	int bitop_pcnt3 = crstl::popcount((uint16_t)0xffu);
	crstl_check(bitop_pcnt3 == 8);

	int bitop_pcnt4 = crstl::popcount((uint32_t)0xffffffu);
	crstl_check(bitop_pcnt4 == 24);

	int bitop_pcnt5 = crstl::popcount((uint64_t)0xffffffffffu);
	crstl_check(bitop_pcnt5 == 40);

	// rotl

	uint8_t bitop_rotl0 = crstl::rotl((uint8_t)129, 1);
	crstl_check(bitop_rotl0 == 3);

	uint16_t bitop_rotl1 = crstl::rotl((uint16_t)49410, 2);
	crstl_check(bitop_rotl1 == 1035);

	uint32_t bitop_rotl2 = crstl::rotl((uint32_t)262155, 4);
	crstl_check(bitop_rotl2 == 4194480);

	uint64_t bitop_rotl3 = crstl::rotl((uint64_t)16939, 6);
	crstl_check(bitop_rotl3 == 1084096);

	// rotr

	uint8_t bitop_rotr0 = crstl::rotr((uint8_t)3, 1);
	crstl_check(bitop_rotr0 == 129);

	uint16_t bitop_rotr1 = crstl::rotr((uint16_t)1035, 2);
	crstl_check(bitop_rotr1 == 49410);

	uint32_t bitop_rotr2 = crstl::rotr((uint32_t)4194480, 4);
	crstl_check(bitop_rotr2 == 262155);

	uint64_t bitop_rotr3 = crstl::rotr((uint64_t)1084096, 6);
	crstl_check(bitop_rotr3 == 16939);

	// byteswap

	uint8_t bitop_bswap0 = crstl::byteswap((uint8_t)144);
	crstl_check(bitop_bswap0 == 144);

	uint16_t bitop_bswap1 = crstl::byteswap((uint16_t)6712);
	crstl_check(bitop_bswap1 == 14362);

	uint32_t bitop_bswap2 = crstl::byteswap((uint32_t)8965);
	crstl_check(bitop_bswap2 == 86179840);

	uint64_t bitop_bswap3 = crstl::byteswap((uint64_t)98745621);
	crstl_check(bitop_bswap3 == 1566656756497514496);

	// bitcast

	uint32_t bitop_bcfu = crstl::bitcast<uint32_t>(2.0f);
	crstl_check(bitop_bcfu == 1073741824);

	float bitop_bcuf = crstl::bitcast<float>(1073741824);
	crstl_check(bitop_bcuf == 2.0f);
}
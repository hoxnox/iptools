/**@author hoxnox <hoxnox@gmail.com>
 * @date $date$*/

#include <iptools/cidr_v6.hpp>
#include <sstream>

using namespace iptools;

inline const std::array<uint8_t, 16>
mkaddr(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5,
       uint8_t a6, uint8_t a7, uint8_t a8, uint8_t a9, uint8_t aa, uint8_t ab,
       uint8_t ac, uint8_t ad, uint8_t ae, uint8_t af)
{
	return {a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, aa, ab, ac, ad, ae, af};
}

TEST(test_cidr_v6, from_string)
{
	cidr_v6 addr("::1/64");
	EXPECT_EQ(mkaddr(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1), (std::array<uint8_t, 16>)addr);
	EXPECT_EQ(64, addr.mask());
}

TEST(test_cidr_v6, str)
{
	cidr_v6 addr(mkaddr(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1), 64);
	std::stringstream ss;
	ss << addr;
	EXPECT_EQ("::1/64", ss.str());
}

TEST(test_cidr_v6, first_last)
{
	cidr_v6 addr("::1/60");
	EXPECT_EQ(mkaddr(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1), addr.first());
	EXPECT_EQ(mkaddr(0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF), addr.last());
}

TEST(test_cidr_v6, first_last_one_byte)
{
	cidr_v6 addr("::FFF8/125");
	EXPECT_EQ(mkaddr(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xF9), addr.first());
	EXPECT_EQ(mkaddr(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF), addr.last());
}

TEST(test_cidr_v6, operator_eq)
{
	cidr_v6 a1("::1/24");
	cidr_v6 a2("::2/24");
	cidr_v6 a3("::2/24");
	cidr_v6 a4("::2/23");
	cidr_v6 a5("FF::1/24");
	EXPECT_NE(a1, a2);
	EXPECT_NE(a1, a3);
	EXPECT_NE(a1, a4);
	EXPECT_NE(a1, a5);
	EXPECT_EQ(a2, a3);
	EXPECT_NE(a2, a4);
	EXPECT_NE(a2, a5);
	EXPECT_NE(a3, a4);
	EXPECT_NE(a3, a5);
	EXPECT_NE(a4, a5);
}


TEST(test_cidr_v6, is_net)
{
	EXPECT_FALSE(cidr_v6("::1/32").is_net());
	EXPECT_FALSE(cidr_v6("::1/24").is_net());
	EXPECT_TRUE (cidr_v6("::FF:FF00/120").is_net());
	EXPECT_TRUE (cidr_v6("FF::0/120").is_net());
	EXPECT_TRUE (cidr_v6("::FFF8/125").is_net());
	EXPECT_FALSE(cidr_v6("::FFF8/124").is_net());
	EXPECT_TRUE (cidr_v6("::0/0").is_net());
}


TEST(test_cidr_v6, net)
{
	EXPECT_EQ(cidr_v6("::FFF8/125"), cidr_v6("::FFFF/125").net());
	EXPECT_NE(cidr_v6("::FFF8/125").net(), cidr_v6("::FFF8/126").net());
}

TEST(test_cidr_v6, in)
{
	EXPECT_TRUE (cidr_v6("1::1/128").in(cidr_v6("1::0/104")));
	EXPECT_TRUE (cidr_v6("1::1/112").in(cidr_v6("1::0/112")));
	EXPECT_FALSE(cidr_v6("::1/24").in(cidr_v6("::2/24")));
	EXPECT_TRUE (cidr_v6("::1/24").in(cidr_v6("::1/24")));
	EXPECT_TRUE (cidr_v6("::FFF8/125").net().in(cidr_v6("::FFF0/124")));
	EXPECT_TRUE (cidr_v6("::1/24").in(cidr_v6("::0/0")));
}

TEST(test_cidr_v6, check_bit)
{
	auto addr = cidr_v6("800a::f0:5/128"); // 1000000000001010::0000000011110000'0000000000000101
	for (uint8_t i = 0; i < 128+1; ++i)
	{
		if (i == 0 || i== 2 || i == 20 || i == 21 || i == 22 || i == 23 || i == 113 || i == 115 || i == 127)
		{
			EXPECT_TRUE (addr.check_bit(i)) << (int)i;
			continue;
		}
		EXPECT_FALSE (addr.check_bit(i)) << (int)i;
	}
}

TEST(test_cidr_v6, has_prefix)
{
	auto addr = cidr_v6("a000::f0:1/128");
	EXPECT_TRUE (addr.has_prefix(std::array<uint8_t, 16>{
			0xa0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 8));
	EXPECT_FALSE (addr.has_prefix(std::array<uint8_t, 16>{
			0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 8));
	EXPECT_FALSE (addr.has_prefix(std::array<uint8_t, 16>{
			0xa0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 16));
}

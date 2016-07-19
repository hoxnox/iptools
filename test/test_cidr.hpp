/**@author hoxnox <hoxnox@gmail.com>
 * @date 20160405 11:39:23 */

#include <iptools/cidr.hpp>
#include <sstream>

using namespace iptools;

TEST(test_cidr_v4, localhost)
{
	cidr_v4 addr("127.0.0.5/24");
	EXPECT_EQ(2130706437, (uint32_t)addr);
	EXPECT_EQ(2130706432, addr.first());
	EXPECT_EQ(2130706687, addr.last());
	std::stringstream ss;
	ss << addr;
	EXPECT_EQ("127.0.0.5/24", ss.str());
	cidr_v4 addr1("127.0.0.5");
	EXPECT_EQ(2130706437, (uint32_t)addr1);
	EXPECT_EQ(32, addr1.mask());
}

TEST(test_cidr_v4, privateB)
{
	cidr_v4 addr("172.16.1.2/12");
	EXPECT_EQ(2886729986, (uint32_t)addr);
	EXPECT_EQ(2886729728, addr.first());
	EXPECT_EQ(2887778303, addr.last()) << "172.31.255.255";
	std::stringstream ss;
	ss << addr;
	EXPECT_EQ("172.16.1.2/12", ss.str());
}

TEST(test_cidr_v4, mask32)
{
	cidr_v4 addr("213.162.1.255/32");
	EXPECT_EQ(3584164351, (uint32_t)addr);
	EXPECT_EQ(3584164351, addr.first());
	EXPECT_EQ(3584164351, addr.last());
	std::stringstream ss;
	ss << addr;
	EXPECT_EQ("213.162.1.255/32", ss.str());
}

TEST(test_cidr_v4, operator_eq)
{
	cidr_v4 a1("127.0.0.1/24");
	cidr_v4 a2("127.0.0.2/24");
	cidr_v4 a3("127.0.0.2/24");
	cidr_v4 a4("127.0.0.2/23");
	cidr_v4 a5("128.0.0.1/24");
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

TEST(test_cidr_v4, is_net)
{
	EXPECT_FALSE(cidr_v4("127.0.0.1/32"  ).is_net());
	EXPECT_FALSE(cidr_v4("127.0.0.1/24"  ).is_net());
	EXPECT_TRUE (cidr_v4("127.0.0.0/24"  ).is_net());
	EXPECT_TRUE (cidr_v4("127.0.255.0/24").is_net());
	EXPECT_TRUE (cidr_v4("127.255.0.0/16").is_net());
	EXPECT_FALSE(cidr_v4("127.255.2.0/16").is_net());
	EXPECT_TRUE (cidr_v4("0.0.0.0/0"     ).is_net());
}

TEST(test_cidr_v4, net)
{
	EXPECT_EQ(cidr_v4("127.0.0.0/24"), cidr_v4("127.0.0.5/24").net());
	EXPECT_NE(cidr_v4("127.0.0.5/23").net(), cidr_v4("127.0.0.5/24").net());
}

TEST(test_cidr_v4, in)
{
	EXPECT_TRUE (cidr_v4("10.0.0.1/32").in(cidr_v4("10.0.0.0/24")));
	EXPECT_TRUE (cidr_v4("127.0.0.1/24").in(cidr_v4("127.0.0.0/24")));
	EXPECT_FALSE(cidr_v4("127.0.0.1/24").in(cidr_v4("127.0.0.2/24")));
	EXPECT_TRUE (cidr_v4("127.0.0.1/24").in(cidr_v4("127.0.0.1/24")));
	EXPECT_TRUE (cidr_v4("127.0.1.0/24").net().in(cidr_v4("127.0.0.0/16")));
}

TEST(test_cidr_v4, iterator_simple)
{
	std::stringstream etalon;
	etalon << "192.168.10.1/30" << std::endl
	       << "192.168.10.2/30" << std::endl
	       << "192.168.10.3/30" << std::endl;
	std::stringstream result;
	for (auto i : cidr_v4("192.168.10.0/30"))
		result << i << std::endl;
	ASSERT_EQ(etalon.str(), result.str());
}

TEST(test_cidr_v4, iterator_not_network)
{
	std::stringstream etalon;
	etalon << "10.0.0.1/8" << std::endl;
	std::stringstream result;
	for (auto i : cidr_v4("10.0.0.1/8"))
		result << i << std::endl;
	ASSERT_EQ(etalon.str(), result.str());
}

TEST(test_cidr_v4, iterator_last)
{
	std::stringstream etalon;
	etalon << "255.255.255.255/32" << std::endl;
	std::stringstream result;
	for (auto i : cidr_v4("255.255.255.255/32"))
		result << i << std::endl;
	EXPECT_EQ(etalon.str(), result.str());
	auto last = cidr_v4("0.0.0.0/0").begin() + (0x1000000ULL*255 + 0x10000*255 + 0x100*255 + 255 - 1);
	EXPECT_TRUE(cidr_v4("255.255.255.255/0") == *last) << *last;
	EXPECT_TRUE(last != cidr_v4("0.0.0.0/0").end());
	++last;
	EXPECT_TRUE(last == cidr_v4("0.0.0.0/0").end());
}

TEST(test_cidr_v4, iterator_first)
{
	std::stringstream etalon;
	etalon << "0.0.0.1/31" << std::endl;
	std::stringstream result;
	for (auto i : cidr_v4("0.0.0.0/31"))
		result << i << std::endl;
	ASSERT_EQ(etalon.str(), result.str());
}

TEST(test_cidr_v4, iterator_eq)
{
	auto a = cidr_v4("1.0.0.1/8").begin();
	auto b = cidr_v4("1.0.0.1/7").begin();
	EXPECT_TRUE(a == a);
	EXPECT_TRUE(a != b);
}

TEST(test_cidr_v4, iterator_moving)
{
	auto i = cidr_v4("1.0.0.0/8").begin();
	EXPECT_EQ(cidr_v4("1.0.0.10/8"), *(i+9));
	EXPECT_EQ(cidr_v4("1.0.0.2/8"), *(++i));
	EXPECT_EQ(cidr_v4("1.0.0.2/8"), *i++);
	EXPECT_EQ(cidr_v4("1.0.0.3/8"), *i);
}

TEST(test_cidr_v4, iterator_moving_not_net)
{
	auto i = cidr_v4("1.0.0.1/8").begin();
	EXPECT_TRUE(cidr_v4("1.0.0.1/8").end() == i+10);
	EXPECT_TRUE(cidr_v4("1.0.0.1/8").end() == ++i);
	EXPECT_TRUE(cidr_v4("1.0.0.1/8").end() == i++);
	EXPECT_TRUE(cidr_v4("1.0.0.1/8").end() == i);
}

TEST(test_cidr_v4, distance_whole)
{
	ASSERT_EQ(0xffffffffUL, std::distance(cidr_v4("0.0.0.0/0").begin(),
	                                       cidr_v4("0.0.0.0/0").end()));
}


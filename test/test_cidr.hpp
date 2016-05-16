/**@author hoxnox <hoxnox@gmail.com>
 * @date 20160405 11:39:23 */

#include <iptools/cidr.hpp>
#include <sstream>

using namespace rators;

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
	EXPECT_TRUE (cidr_v4("127.0.0.1/24").in(cidr_v4("127.0.0.0/24")));
	EXPECT_FALSE(cidr_v4("127.0.0.1/24").in(cidr_v4("127.0.0.2/24")));
	EXPECT_TRUE (cidr_v4("127.0.0.1/24").in(cidr_v4("127.0.0.1/24")));
	EXPECT_TRUE (cidr_v4("127.0.1.0/24").net().in(cidr_v4("127.0.0.0/16")));
}


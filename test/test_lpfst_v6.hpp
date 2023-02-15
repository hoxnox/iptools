/**@author hoxnox <hoxnox@gmail.com>
 * @date 20230215 18:19:26*/

#include <iptools/lpfst_v6.hpp>
#include <fstream>

using namespace iptools;

TEST(test_lpfst_v6, simple_check_sidr)
{
	basic_lpfst_v6<std::string> ipset;
	cidr_v6 addr;
	//clang-format off
	addr = cidr_v6{"2001:320:4002:2000::/64"      }; ipset.insert(addr, addr.bstr());
	addr = cidr_v6{"2001:67c:2204:607::2ffa:0/128"}; ipset.insert(addr, addr.bstr());
	addr = cidr_v6{"2001:808::/35"                }; ipset.insert(addr, addr.bstr());
	addr = cidr_v6{"2001:838::/32"                }; ipset.insert(addr, addr.bstr());
	addr = cidr_v6{"fd00:10:130:151::254/128"     }; ipset.insert(addr, addr.bstr());
	addr = cidr_v6{"2801:830::/34"                }; ipset.insert(addr, addr.bstr());
	addr = cidr_v6{"2001:df2:7180::14/126"        }; ipset.insert(addr, addr.bstr());
	addr = cidr_v6{"2001:830::/32"                }; ipset.insert(addr, addr.bstr());
	addr = cidr_v6{"fd00:10:130:151::10/127"      }; ipset.insert(addr, addr.bstr());
	//clang-format on
	std::cout << ipset.print();
}

TEST(tgest_lpfst_v6, remove_leafs)
{
	basic_lpfst_v6<std::string> ipset;
	for (size_t j = 1; j <= 2; j++)
	{
		for (size_t i = 0; i <= 0xff; i++)
		{
			in6_addr_t addr{};
			addr[15-j] = i;
			//ipset.insert({addr, 128-8}, print_binary(addr));
		}
	}
//	ipset.insert({"1111:1111:1111:0/64" }, "a");
//	ipset.insert({"1111:1111:1111:0/64" }, "b");   //        e
//	ipset.insert({"1111:1111:1110:0/48" }, );   //      c   b
//	ipset.insert({"1111:1111:1110:0/47" }, );   //   d        f
//	ipset.insert({"1111:1111:1111:0/48" }, );   // a        g
//	ipset.insert({"1111:1111:1111:0/65" }, "f");   //
//	ipset.insert({"1111:1111:1111:0/65" }, "g");
	std::cout << ipset.print();

	/*
	std::string rs;
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("215.1.2.1")), rs));
	EXPECT_EQ   ("g", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.0.0.1")), rs));
	EXPECT_EQ   ("a", rs);

	ipset.remove({"10.0.0.0/8"});
	ipset.remove({"215.1.2.0/24"});

	rs.clear();
	EXPECT_TRUE(ipset.check(ntohl(inet_addr("127.0.0.1")), rs));
	EXPECT_EQ   ("c", rs);

	rs.clear();
	EXPECT_FALSE (ipset.check(ntohl(inet_addr("215.1.2.1")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_FALSE (ipset.check(ntohl(inet_addr("215.1.2.2")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_FALSE (ipset.check(ntohl(inet_addr("215.1.2.255")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("215.1.3.2")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_FALSE (ipset.check(ntohl(inet_addr("10.0.0.1")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_FALSE (ipset.check(ntohl(inet_addr("10.0.1.0")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_FALSE (ipset.check(ntohl(inet_addr("10.255.255.255")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("213.1.2.1")), rs));
	EXPECT_EQ   ("f", rs);

	rs.clear();
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("11.0.0.1")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("192.168.1.1")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("192.168.2.1")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("192.168.3.1")), rs));
	EXPECT_EQ   ("b", rs);

	rs.clear();
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("192.168.4.1")), rs));
	EXPECT_EQ   ("", rs);
	*/
}

/** the code below helped to make tests
std::string
print(const basic_lpfst_v6<std::string>& ipset, const cidr_v6& addr)
{
	std::stringstream ss;
	std::string aux;
	in6_addr_t addr_ = (in6_addr_t)addr;
	if (ipset.check(addr, aux))
	{
		ss << addr << std::endl;
		ss << "+" << aux << std::endl << " " << print_binary(addr_, addr.mask()) << std::endl;
	}
	else
		ss << "-" << print_binary(addr_, addr.mask()) << " " << addr;
	return ss.str();
}

TEST(tgest_lpfst_v6, all_ipv6_inet)
{

	// IPv6 prefixes from big view of Internet routing
	std::ifstream ifile{"/home/hoxnox/tmp/20230217.bview.ipv6.prefixes"};
	std::string line;
	basic_lpfst_v6<std::string> ipset;
	size_t max_mask = 0;
	srand(42);
	std::vector<cidr_v6> addrs;
	while (getline(ifile, line))
	{
		auto addr = cidr_v6{line.substr(8, line.length() - 8)};
		ipset.insert(addr, print_binary(addr, addr.mask()));

		// get random address from the network
		if (addr.mask() == 128)
		{
			addrs.emplace_back(addr);
			continue;
		}
		in6_addr_t addr_ = (in6_addr_t)addr;
		auto r = rand()%(128-addr.mask());
		addr_ = set_bit(addr_, r, true);
		addrs.emplace_back(addr_, addr.mask());
	}
	std::cout << max_mask << std::endl;
	std::cout << ipset.print();
	for (const auto a : addrs)
		std::cout << print(ipset, a) << std::endl;
}
/***/

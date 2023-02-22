/**@author hoxnox <hoxnox@gmail.com>
 * @date 20230215 18:19:26*/

#include "iptools/cidr_v6.hpp"
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

	std::string dbg;

	EXPECT_TRUE (ipset.check(cidr_v6{"2001:320:4002:2000:ab::"  }, dbg)) << dbg;
	EXPECT_TRUE (ipset.check(cidr_v6{"2001:67c:2204:607::2ffa:0"}, dbg)) << dbg;
	EXPECT_TRUE (ipset.check(cidr_v6{"2001:808::1"              }, dbg)) << dbg;
	EXPECT_TRUE (ipset.check(cidr_v6{"2001:838:11::/64"         }, dbg)) << dbg;
	EXPECT_TRUE (ipset.check(cidr_v6{"fd00:10:130:151::254"     }, dbg)) << dbg;
	EXPECT_TRUE (ipset.check(cidr_v6{"2801:830::1/64"           }, dbg)) << dbg;
	EXPECT_TRUE (ipset.check(cidr_v6{"2001:df2:7180::15"        }, dbg)) << dbg;
	EXPECT_TRUE (ipset.check(cidr_v6{"2001:830:ffff::/120"      }, dbg)) << dbg;
	EXPECT_TRUE (ipset.check(cidr_v6{"fd00:10:130:151::11"      }, dbg)) << dbg;

	EXPECT_FALSE(ipset.check(cidr_v6{"2001:320:4002:2001/128"   }, dbg)) << dbg;
	EXPECT_FALSE(ipset.check(cidr_v6{"2001:67c:2204:607::2ffa:1"}, dbg)) << dbg;
	EXPECT_FALSE(ipset.check(cidr_v6{"2002:808::1"              }, dbg)) << dbg;
	EXPECT_FALSE(ipset.check(cidr_v6{"2001:837::/64"            }, dbg)) << dbg;
	EXPECT_FALSE(ipset.check(cidr_v6{"ed00:10:130:151::254"     }, dbg)) << dbg;
	EXPECT_FALSE(ipset.check(cidr_v6{"2801:831::1/64"           }, dbg)) << dbg;
	EXPECT_FALSE(ipset.check(cidr_v6{"2001:df2:7180::13"        }, dbg)) << dbg;
	EXPECT_FALSE(ipset.check(cidr_v6{"2001:830::/31"            }, dbg)) << dbg;
	EXPECT_FALSE(ipset.check(cidr_v6{"fd00:10:130:151::12"      }, dbg)) << dbg;
	//std::cout << ipset.print();
}

TEST(tgest_lpfst_v6, remove_leafs)
{
	basic_lpfst_v6<std::string> ipset;
	cidr_v6 addr;

	//clang-format off
	addr = cidr_v6{"2001:320:4002:2000::/64"      }; ipset.insert(addr, addr.bstr());  // fd00:10:130:151::254/128 [11111101'00000000'00000000'00010000'00000001'00110000'00000001'01010001'00000000'00000000'00000000'00000000'00000000'00000000'00000010'01010100]
	addr = cidr_v6{"2001:67c:2204:607::2ffa:0/128"}; ipset.insert(addr, addr.bstr());  // 1  [-] 2001:67c:2204:607::2ffa:0/128 [00100000'00000001'00000110'01111100'00100010'00000100'00000110'00000111'00000000'00000000'00000000'00000000'00101111'11111010'00000000'00000000]
	addr = cidr_v6{"2001:808::/35"                }; ipset.insert(addr, addr.bstr());  // 2    [-] 2001:df2:7180::14/126 [00100000'00000001'00001101'11110010'01110001'10000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'000101]00
	addr = cidr_v6{"2001:838::/32"                }; ipset.insert(addr, addr.bstr());  // 3      [+] 2001:320:4002:2000::/64 [00100000'00000001'00000011'00100000'01000000'00000010'00100000'00000000]'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000
	addr = cidr_v6{"fd00:10:130:151::254/128"     }; ipset.insert(addr, addr.bstr());  // 4        [-] 2001:808::/35 [00100000'00000001'00001000'00001000'000]00000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000
	addr = cidr_v6{"2801:830::/34"                }; ipset.insert(addr, addr.bstr());  // 5          [-] 2001:830::/34 [00100000'00000001'00001000'00110000'00]000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000
	addr = cidr_v6{"2001:df2:7180::14/126"        }; ipset.insert(addr, addr.bstr());  // 6            [-] 2001:830::/32 [00100000'00000001'00001000'00110000]'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000
	addr = cidr_v6{"2001:830::/34"                }; ipset.insert(addr, addr.bstr());  // 7              [-] 2001:838::/32 [00100000'00000001'00001000'00111000]'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000
	addr = cidr_v6{"2001:830::/32"                }; ipset.insert(addr, addr.bstr());  // 5          [+] 2801:830::/34 [00101000'00000001'00001000'00110000'00]000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000
	addr = cidr_v6{"fd00:10:130:151::10/127"      }; ipset.insert(addr, addr.bstr());  // 1  [+] fd00:10:130:151::10/127 [11111101'00000000'00000000'00010000'00000001'00110000'00000001'01010001'00000000'00000000'00000000'00000000'00000000'00000000'00000000'0001000]0
	//clang-format on

	std::string dbg;

	ipset.remove(cidr_v6{"2001:320:4002:2000::/64"});
	EXPECT_FALSE(ipset.check(cidr_v6{"2001:320:4002:2000:ab::"}, dbg)) << dbg;
	ipset.remove(cidr_v6{"2001:808::/35"});
	EXPECT_FALSE(ipset.check(cidr_v6{"2001:808::1"}, dbg)) << dbg;
	ipset.remove(cidr_v6{"2001:838::/32"});
	EXPECT_FALSE(ipset.check(cidr_v6{"2001:838:11::/64"}, dbg)) << dbg;

	EXPECT_TRUE(ipset.check(cidr_v6{"2001:830:0:0:a000::"}, dbg)) << dbg;
	ipset.remove(cidr_v6{"2001:830::/32"});
	EXPECT_TRUE(ipset.check(cidr_v6{"2001:830:2000::"}, dbg)) << cidr_v6{"2001:830:2000::"}.bstr();
	EXPECT_FALSE(ipset.check(cidr_v6{"2001:830:6000::"}, dbg)) << dbg;
	ipset.remove(cidr_v6{"2001:830::/34"});
	EXPECT_FALSE(ipset.check(cidr_v6{"2001:830:2000::"}, dbg)) << dbg;
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

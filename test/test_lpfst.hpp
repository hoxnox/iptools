/**@author hoxnox <hoxnox@gmail.com>
 * @date 20160411 11:31:17*/

#include <iptools/lpfst.hpp>

using namespace rators;

TEST(test_lpfst, simple)
{
	lpfst ipset;
	ipset.insert({"10.0.0.0/8"    });
	ipset.insert({"192.168.3.0/24"});
	ipset.insert({"127.0.0.1/24"  });
	ipset.insert({"10.0.2.0/24"   });
	ipset.insert({"213.1.2.0/24"   });
	ipset.insert({"215.1.2.0/24"   });

	EXPECT_TRUE (ipset.check({"215.1.2.1/24"     }));
	EXPECT_TRUE (ipset.check({"215.1.2.255/24"   }));
	EXPECT_TRUE (ipset.check({"215.1.2.255/8"    }));
	EXPECT_FALSE(ipset.check({"215.0.0.0/8"      }));
	EXPECT_TRUE (ipset.check({"10.0.0.1/24"      }));
	EXPECT_TRUE (ipset.check({"10.255.255.255/24"}));
	EXPECT_TRUE (ipset.check({"10.255.255.255/8" }));
	EXPECT_TRUE (ipset.check({"10.0.0.0/24"      }));
	EXPECT_TRUE (ipset.check({"213.1.2.1/24"     }));
	EXPECT_FALSE(ipset.check({"10.0.0.0/7"       }));
	EXPECT_FALSE(ipset.check({"10.0.0.0/7"       }));
	EXPECT_FALSE(ipset.check({"11.0.0.0/8"       }));
	EXPECT_FALSE(ipset.check({"192.168.1.1/8"    }));
	EXPECT_FALSE(ipset.check({"192.168.1.1/24"   }));
}

TEST(test_lpfst, total_16)
{
	lpfst ipset;
	ipset.insert({"219.189.24.0/24" });
	ipset.insert({"219.189.28.2/32" });
	ipset.insert({"219.189.176.0/24"});
	ipset.insert({"219.189.42.0/24" });
	ipset.insert({"219.189.43.1/32" });
	ipset.insert({"219.189.192.0/24"});
	ipset.insert({"219.189.2.0/24"  });
	ipset.insert({"219.189.6.0/24"  });

	uint32_t le_pref = 219*0x1000000U + 189*0x10000;
	for (size_t i = 0; i < 0xff; ++i)
	{
		uint32_t le_secpref = le_pref + i*0x100;
		 for (size_t j = 0; j < 0xff; ++j)
		 {
			uint32_t le_addr = le_secpref + j;
			cidr_v4 addr(le_addr, 32);
			if (i == 24 || i == 176 || i == 42 || i == 192 || i == 2 || i == 6) 
				EXPECT_TRUE(ipset.check(addr)) << addr;
			else if (i == 28 && j == 2)
				EXPECT_TRUE(ipset.check(addr)) << addr;
			else if (i == 43 && j == 1)
				EXPECT_TRUE(ipset.check(addr)) << addr;
			else
				EXPECT_FALSE(ipset.check(addr)) << addr;
		 }
	}
}


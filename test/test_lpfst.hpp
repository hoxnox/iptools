/**@author hoxnox <hoxnox@gmail.com>
 * @date 20160411 11:31:17*/

#include <iptools/lpfst.hpp>

using namespace iptools;

TEST(test_lpfst, simple_check_sidr)
{
	lpfst ipset;
	ipset.insert({"10.0.0.0/8"    });
	ipset.insert({"192.168.3.0/24"});
	ipset.insert({"127.0.0.1/24"  });
	ipset.insert({"10.0.2.0/24"   });
	ipset.insert({"213.1.2.0/24"  });
	ipset.insert({"215.1.2.0/24"  });

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

TEST(test_lpfst, simple_check_uint32)
{
	lpfst ipset;
	ipset.insert({"10.0.0.0/8"    });
	ipset.insert({"192.168.3.0/24"});
	ipset.insert({"127.0.0.1/24"  });
	ipset.insert({"10.0.2.0/24"   });
	ipset.insert({"213.1.2.0/24"  });
	ipset.insert({"215.1.2.0/24"  });

	EXPECT_TRUE (ipset.check(ntohl(inet_addr("215.1.2.1"     ))));
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("215.1.2.2"     ))));
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("215.1.2.255"   ))));
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("215.1.3.2"     ))));
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.0.0.1"      ))));
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.0.1.0"      ))));
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.255.255.255"))));
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("213.1.2.1"     ))));
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("11.0.0.1"      ))));
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("192.168.1.1"   ))));
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("192.168.2.1"   ))));
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("192.168.3.1"   ))));
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("192.168.4.1"   ))));
}

TEST(test_lpfst, empty)
{
	lpfst ipset;
	EXPECT_TRUE(ipset.empty());
	ipset.insert({"10.0.0.0/8"});
	EXPECT_FALSE(ipset.empty());
}

TEST(test_lpfst, clear)
{
	lpfst ipset;
	ipset.insert({"10.0.0.0/8"});
	ipset.insert({"192.168.3.0/24"});
	ipset.clear();
	EXPECT_FALSE(ipset.check(inet_addr("10.0.0.1")));
	EXPECT_TRUE(ipset.empty());
}


TEST(test_lpfst, copy)
{
	std::unique_ptr<lpfst> ipset1(new lpfst);
	ipset1->insert({"10.0.0.0/8"    });
	ipset1->insert({"192.168.3.0/24"});
	ipset1->insert({"127.0.0.1/24"  });
	ipset1->insert({"10.0.2.0/24"   });
	ipset1->insert({"213.1.2.0/24"  });
	ipset1->insert({"215.1.2.0/24"  });

	std::unique_ptr<lpfst> ipset2(new lpfst(*ipset1));
	ipset1.reset(nullptr);

	EXPECT_TRUE (ipset2->check({"215.1.2.1/24"     }));
	EXPECT_TRUE (ipset2->check({"215.1.2.255/24"   }));
	EXPECT_TRUE (ipset2->check({"215.1.2.255/8"    }));
	EXPECT_FALSE(ipset2->check({"215.0.0.0/8"      }));
	EXPECT_TRUE (ipset2->check({"10.0.0.1/24"      }));
	EXPECT_TRUE (ipset2->check({"10.255.255.255/24"}));
	EXPECT_TRUE (ipset2->check({"10.255.255.255/8" }));
	EXPECT_TRUE (ipset2->check({"10.0.0.0/24"      }));
	EXPECT_TRUE (ipset2->check({"213.1.2.1/24"     }));
	EXPECT_FALSE(ipset2->check({"10.0.0.0/7"       }));
	EXPECT_FALSE(ipset2->check({"10.0.0.0/7"       }));
	EXPECT_FALSE(ipset2->check({"11.0.0.0/8"       }));
	EXPECT_FALSE(ipset2->check({"192.168.1.1/8"    }));
	EXPECT_FALSE(ipset2->check({"192.168.1.1/24"   }));
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

TEST(TestLPFST, remove_leafs)
{
	basic_lpfst<std::string> ipset;
	ipset.insert({"10.0.0.0/8"    }, "a");
	ipset.insert({"192.168.3.0/24"}, "b");  //         e
	ipset.insert({"127.0.0.0/24"  }, "c");  //      c    b
	ipset.insert({"10.0.2.0/24"   }, "d");  //   d         f
	ipset.insert({"10.0.2.128/25" }, "e");  // a        g
	ipset.insert({"213.1.2.0/24"  }, "f");  //
	ipset.insert({"215.1.2.0/24"  }, "g");

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
}

TEST(TestLPFST, remove_with_child)
{
	basic_lpfst<std::string> ipset;
	ipset.insert({"10.0.0.0/8"	}, "a");
	ipset.insert({"192.168.3.0/24"}, "b");
	ipset.insert({"127.0.0.0/24"  }, "c");
	ipset.insert({"10.0.2.0/24"   }, "d");
	ipset.insert({"10.0.2.128/25" }, "e");
	ipset.insert({"213.1.2.0/24"  }, "f");
	ipset.insert({"215.1.2.0/24"  }, "g");

	std::string rs;
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("192.168.3.1")), rs));
	EXPECT_EQ   ("b", rs);

	ipset.remove({"192.168.3.0/24"});

	rs.clear();
	EXPECT_FALSE (ipset.check(ntohl(inet_addr("192.168.3.1")), rs));
	EXPECT_EQ   ("", rs);

	// other were not affected

	rs.clear();
	EXPECT_TRUE(ipset.check(ntohl(inet_addr("127.0.0.1")), rs));
	EXPECT_EQ   ("c", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("215.1.2.1")), rs));
	EXPECT_EQ   ("g", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("215.1.2.2")), rs));
	EXPECT_EQ   ("g", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("215.1.2.255")), rs));
	EXPECT_EQ   ("g", rs);

	rs.clear();
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("215.1.3.2")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.0.0.1")), rs));
	EXPECT_EQ   ("a", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.0.1.0")), rs));
	EXPECT_EQ   ("a", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.255.255.255")), rs));
	EXPECT_EQ   ("a", rs);

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
	EXPECT_FALSE (ipset.check(ntohl(inet_addr("192.168.3.1")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("192.168.4.1")), rs));
	EXPECT_EQ   ("", rs);
}

TEST(TestLPFST, data_rewrite)
{
	basic_lpfst<std::string> ipset;
	ipset.insert({"10.0.0.0/8"    }, "a");
	std::string rs;
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.0.0.1")), rs));
	EXPECT_EQ   ("a", rs);

	ipset.insert({"10.0.0.0/8"    }, "b");
	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.0.0.1")), rs));
	EXPECT_EQ   ("b", rs);
}

TEST(TestLPFST, remove_root)
{
	basic_lpfst<std::string> ipset;
	ipset.insert({"10.0.0.0/8"    }, "a");
	ipset.insert({"192.168.3.0/24"}, "b");
	ipset.insert({"127.0.0.0/24"  }, "c");
	ipset.insert({"10.0.2.0/24"   }, "d");
	ipset.insert({"10.0.2.128/25" }, "e");
	ipset.insert({"213.1.2.0/24"  }, "f");
	ipset.insert({"215.1.2.0/24"  }, "g");

	std::string rs;
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.0.2.129")), rs));
	EXPECT_EQ   ("e", rs);

	ipset.remove({"10.0.2.128/25"});

	rs.clear();
	EXPECT_TRUE(ipset.check(ntohl(inet_addr("10.0.2.129")), rs));
	EXPECT_EQ  ("d", rs);

	// other were not affected

	rs.clear();
	EXPECT_TRUE(ipset.check(ntohl(inet_addr("127.0.0.1")), rs));
	EXPECT_EQ   ("c", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("215.1.2.1")), rs));
	EXPECT_EQ   ("g", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("215.1.2.2")), rs));
	EXPECT_EQ   ("g", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("215.1.2.255")), rs));
	EXPECT_EQ   ("g", rs);

	rs.clear();
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("215.1.3.2")), rs));
	EXPECT_EQ   ("", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.0.0.1")), rs));
	EXPECT_EQ   ("a", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.0.1.0")), rs));
	EXPECT_EQ   ("a", rs);

	rs.clear();
	EXPECT_TRUE (ipset.check(ntohl(inet_addr("10.255.255.255")), rs));
	EXPECT_EQ   ("a", rs);

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
	EXPECT_TRUE(ipset.check(ntohl(inet_addr("192.168.3.1")), rs));
	EXPECT_EQ  ("b", rs);

	rs.clear();
	EXPECT_FALSE(ipset.check(ntohl(inet_addr("192.168.4.1")), rs));
	EXPECT_EQ   ("", rs);
}

TEST(test_lpfst, internet_blacklist)
{
	lpfst inet_bl = internet_blacklist();

	cidr_v4 full("0.0.0.0/0");
	auto cur = full.begin();
	while(inet_bl.check(*cur)) // 0.0.0.0/8
		++cur;
	EXPECT_TRUE(cidr_v4("1.0.0.0/0") == *cur) << *cur;

	cur += (0x1000000*9 - 1);
	EXPECT_TRUE(cidr_v4("9.255.255.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 10.0.0.0/8
		++cur;
	EXPECT_TRUE(cidr_v4("11.0.0.0/0") == *cur) << *cur;

	cur += (0x1000000*89 + 0x10000*64 - 1);
	EXPECT_TRUE(cidr_v4("100.63.255.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 100.64.0.0/10
		++cur;
	EXPECT_TRUE(cidr_v4("100.128.0.0/0") == *cur) << *cur;

	cur += (0x1000000*69 + 0x10000*126 - 1);
	EXPECT_TRUE(cidr_v4("169.253.255.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 169.254.0.0/16
		++cur;
	EXPECT_TRUE(cidr_v4("169.255.0.0/0") == *cur) << *cur;

	cur += (0x1000000*2 + 0x10000*17 - 1);
	EXPECT_TRUE(cidr_v4("172.15.255.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 172.16.0.0/12
		++cur;
	EXPECT_TRUE(cidr_v4("172.32.0.0/0") == *cur) << *cur;

	cur += (0x1000000*19 + 0x10000*224 - 1);
	EXPECT_TRUE(cidr_v4("191.255.255.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 192.0.0.0/24
		++cur;
	EXPECT_TRUE(cidr_v4("192.0.1.0/0") == *cur) << *cur;

	cur += 255;
	EXPECT_TRUE(cidr_v4("192.0.1.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 192.0.2.0/24
		++cur;
	EXPECT_TRUE(cidr_v4("192.0.3.0/0") == *cur) << *cur;

	cur += (0x10000*17 + 0x100*252 + 255);
	EXPECT_TRUE(cidr_v4("192.17.255.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 192.18.0.0/15
		++cur;
	EXPECT_TRUE(cidr_v4("192.20.0.0/0") == *cur) << *cur;

	cur += (0x10000*68 + 0x100*98 + 255);
	EXPECT_TRUE(cidr_v4("192.88.98.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 192.88.99.0/24
		++cur;
	EXPECT_TRUE(cidr_v4("192.88.100.0/0") == *cur) << *cur;

	cur += (0x10000*79 + 0x100*155 + 255);
	EXPECT_TRUE(cidr_v4("192.167.255.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 192.88.99.0/24
		++cur;
	EXPECT_TRUE(cidr_v4("192.169.0.0/0") == *cur) << *cur;

	cur += (0x1000000*5 +0x10000*138 + 0x100*99 + 255);
	EXPECT_TRUE(cidr_v4("198.51.99.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 198.51.100.0/24
		++cur;
	EXPECT_TRUE(cidr_v4("198.51.101.0/0") == *cur) << *cur;

	cur += (0x1000000*4 +0x10000*205 + 0x100*11 + 255);
	EXPECT_TRUE(cidr_v4("203.0.112.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur)) // 203.0.113.0/24
		++cur;
	EXPECT_TRUE(cidr_v4("203.0.114.0/0") == *cur) << *cur;

	cur += (0x1000000*20 +0x10000*255 + 0x100*141 + 255);
	EXPECT_TRUE(cidr_v4("223.255.255.255/0") == *cur) << *cur;
	++cur;
	while(inet_bl.check(*cur) && cur != cidr_v4("0.0.0.0/0").end()) // 224.0.0.0/3
		cur += 100;
	EXPECT_TRUE(cur == cidr_v4("0.0.0.0/0").end()) << *cur;
}



IP tools helps to work with IP addresses
----------------------------------------

Header only library of IP utilities. Uses features of C++11

## `cidr_v4`

IPv4 address storage. If the address represent network address, you can
iterate over every address in this network.

## Longest Prefix First Search Tree (LPFST)

Data structure allows to add some CIDR networks and check if the given
address belongs to any of them.

## Example

	```C++
	#include <iostream>
	#include <iptools/cidr.hpp>
	#include <iptools/lpfst.hpp>
	
	using namespace iptools;
	
	int main(int argc, char* argv[])
	{
		cidr_v4 local("192.168.1.0/24");
		if (local.is_net())
			std::cout << "Network address: " << local << std::endl;
		for (auto i : local)
		{
			if (i == cidr_v4("192.168.1.100/24"))
			{
				if (!i.is_net())
					std::cout << "Host address: " << i << std::endl;
				if (i.in(local))
					std::cout << "Adress " << i << " is in " << local << std::endl;
			}
		}
	
		lpfst blacklist;
		blacklist.insert({"192.168.1.0/30"});
		blacklist.insert({"192.168.1.100/32"});
		blacklist.insert({"10.2.0.0/16"});
		if (blacklist.check(cidr_v4("192.168.1.2")))
			std::cout << "192.168.1.2 blacklisted" << std::endl;
		if (!blacklist.check(cidr_v4("192.168.1.5")))
			std::cout << "192.168.1.5 is not blacklisted" << std::endl;
		if (blacklist.check(cidr_v4("10.2.0.100")))
			std::cout << "10.2.0.100 is blacklisted" << std::endl;
	
		return 0;
	}
	```

Compilation:
	
	g++ --std=c++11 -I /path/to/iptools/include cidr_example.cpp

Output:

	Network address: 192.168.1.0/24
	Host address: 192.168.1.100/24
	Adress 192.168.1.100/24 is in 192.168.1.0/24
	192.168.1.2 blacklisted
	192.168.1.5 is not blacklisted
	10.2.0.100 is blacklisted



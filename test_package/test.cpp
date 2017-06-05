#include <iptools/cidr.hpp>

int main(int argc, char* argv[])
{
	iptools::cidr_v4 addr("10.0.0.0/8");
	if (addr.is_net())
		return 0;
	return -1;
}


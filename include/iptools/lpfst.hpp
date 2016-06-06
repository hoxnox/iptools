/**@author hoxnox <hoxnox@gmail.com>
 * @date 20160411 11:31:17 */

#pragma once
#include "cidr.hpp"

namespace iptools {

/**@brief Data structure allows to add some CIDR networks and check if
 * the given address belongs to any of them.
 *
 * Based on Longest Prefix First Search Tree (LPFST)*/
class lpfst
{
public:
	void
	insert(cidr_v4 addr)
	{
		nodes.emplace_back(new node(addr));
		if (!root)
		{
			root = nodes.back().get();
			return;
		}
		insert(nodes.back().get(), root, 0);
	}
	bool check(const cidr_v4& addr) const
	{
		node*    y = root;
		uint8_t  level = 0;
		bool     is_net = addr.is_net();
		uint32_t addr_i = (uint32_t)addr;
		uint8_t  mask   = addr.mask();
		while (y != nullptr)
		{
			uint32_t curr_pref = y->prefix>>(32-(y->len));
			if (is_net)
			{
				if (mask < level)
					return true;
				if ((addr_i>>(32-(y->len))) == curr_pref && mask > y->len)
					return true;
			}
			else
			{
				if ((addr_i>>(32-(y->len))) == curr_pref)
					return true;
			}
			if (((addr_i>>(31-level))&1) == 0)
				y=y->left;
			else
				y=y->right;
			++level;
		}
		return false;
	}
private:
	struct node{
		node(const cidr_v4& addr)
			: len(addr.is_net() ? addr.mask() : 32)
			, prefix(addr)
			, left(nullptr)
			, right(nullptr)
		{ }
		uint8_t len;
		uint32_t prefix;
		node* left;
		node* right;
		template<typename T>
		void swap(T& x, T& y)
		{
			x = x^y;
			y = y^x;
			x = x^y;
		}
		void swap(node* rhv)
		{
			swap(len, rhv->len);
			swap(prefix, rhv->prefix);
		}
	};
	std::vector<std::unique_ptr<node>> nodes;
	void insert(node* x, node* y, uint8_t level)
	{
		if (x->len > y->len)
			x->swap(y);
		if (x->len == level)
			return;
		if (((x->prefix>>(31-level))&1) == 0)
		{
			if (y->left == nullptr)
				y->left = x;
			else
				insert(x, y->left, level + 1);
		}
		else
		{
			if (y->right == nullptr)
				y->right = x;
			else
				insert(x, y->right, level + 1);
		}
	}
	node* root = nullptr;
};

inline lpfst
internet_blacklist()
{
	lpfst result;
	// RFC-1122: This host on this network
	result.insert({"0.0.0.0/8"});
	// RFC-6598: Shared address space
	result.insert({"100.64.0.0/10"});
	// RFC-3927: Link local
	result.insert({"169.254.0.0/16"});
	// RFC-1918: Private use
	result.insert({"10.0.0.0/8"});
	// RFC-1918: Private use
	result.insert({"172.16.0.0/12"});
	// RFC-6890: IETF Protocol Assignments
	result.insert({"192.0.0.0/24"});
	// RFC-5737: Documentation (TEST-NET-1)
	result.insert({"192.0.2.0/24"});
	// RFC-3068: 6to4 Relay Anycast
	result.insert({"192.88.99.0/24"});
	// RFC-1918: Private-Use
	result.insert({"192.168.0.0/16"});
	// RFC-2544: Benchmarking
	result.insert({"192.18.0.0/15"});
	// RFC-5737: Documentation (TEST-NET-2)
	result.insert({"198.51.100.0/24"});
	// RFC-5737: Documentation (TEST-NET-3)
	result.insert({"203.0.113.0/24"});
	// RFC-5771: Multicast, RFC-1112: Reserved, RFC-0919: Limited broadcast
	result.insert({"224.0.0.0/3"});
	return result;
}

} // namespace


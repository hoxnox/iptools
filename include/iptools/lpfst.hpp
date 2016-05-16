/**@author hoxnox <hoxnox@gmail.com>
 * @date 20160411 11:31:17 */

#pragma once
#include "cidr.hpp"

namespace rators {

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
		node* y = root;
		uint8_t level = 0;
		bool is_net = addr.is_net();
		while (y != nullptr)
		{
			if (is_net)
			{
				if (addr.mask() < level)
					return true;
				if (((uint32_t)addr>>(32-(y->len))) == (y->prefix>>(32-(y->len)))
				 && addr.mask() > y->len)
					return true;
			}
			else
			{
				if (((uint32_t)addr>>(32-(y->len))) == (y->prefix>>(32-(y->len))))
					return true;
			}
			if ((((uint32_t)addr>>(31-level))&1) == 0)
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

} // namespace


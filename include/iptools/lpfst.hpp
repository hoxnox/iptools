/**@author hoxnox <hoxnox@gmail.com>
 * @date 20160411 11:31:17 */

#pragma once
#include "cidr.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <functional>

namespace iptools {

/**@brief Data structure allows to add some CIDR networks and check if
 * the given address belongs to any of them.
 *
 * Based on Longest Prefix First Search Tree (LPFST)*/
template<class T>
class basic_lpfst
{
public:

	basic_lpfst() : root{nullptr}, size(0) {}

	virtual ~basic_lpfst() { clear(); }

	basic_lpfst(const basic_lpfst& copy)
	{
		operator=(copy);
	}

	basic_lpfst& operator=(const basic_lpfst& copy)
	{
		clear();
		if (!copy.root)
			return *this;
		root.reset(new node(copy.root->len, copy.root->prefix, copy.root->data));
		recurse_copy(copy.root, root);
		size = copy.size;
		return *this;
	}

	void insert(iptools::cidr_v4 addr, T data)
	{
		if (!root)
			root.reset(new node(addr, data));
		else
			insert(addr, data, root, 0);
		++size;
	}

	void remove(const iptools::cidr_v4& addr)
	{
		remove(addr, root, 0);
		--size;
	}

	/**@return true if the address belongs any of the inserted CIDRs*/
	bool check(const iptools::cidr_v4& addr, T& data) const
	{
		node*	y	  = root.get();
		uint8_t  level  = 0;
		bool	 is_net = addr.is_net();
		uint32_t addr_i = (uint32_t)addr;
		uint8_t  mask   = addr.mask();
		while (y != nullptr)
		{
			if (is_net && mask < level)
				return true;
			if (!is_net || (is_net && mask >= y->len))
			{
				uint32_t cmp_mask = ~0;
				cmp_mask <<= 32 - y->len;
				if ((addr_i & cmp_mask) == y->prefix)
				{
					data = y->data;
					return true;
				}
			}
			if ((addr_i & (1 << (31 - level))) == 0)
				y = y->left.get();
			else
				y = y->right.get();
			++level;
		}
		return false;
	}

	/**@return true if the address belongs any of the inserted CIDRs
	 * @param addr in host byte order*/
	bool check(const uint32_t addr, T& data) const
	{
		node*  y = root.get();
		uint8_t  level = 0;
		uint32_t addr_ = addr;
		while (y != nullptr)
		{
			uint32_t cmp_mask = ~0;
			cmp_mask <<= 32 - y->len;
			if ((addr_ & cmp_mask) == y->prefix)
			{
				data = y->data;
				return true;
			}
			if ((addr_ & (1 << (31 - level))) == 0)
				y = y->left.get();
			else
				y = y->right.get();
			++level;
		}
		return false;
	}

	bool empty() const
	{
		return !(bool)root;
	}

	void clear()
	{
		walk(root, 0, nullptr,
			[](node_ptr_t& cur, uint8_t level)
			{
				cur->left.reset(nullptr);
				cur->right.reset(nullptr);
			});
		root.reset(nullptr);
	}

	std::string print()
	{
		if (!root)
			return {};
		std::stringstream ss;
		ss << root->prefix << " " << root->data;
		walk(root, 0, [&ss](node_ptr_t& cur, uint8_t level, bool left)
				{
					ss << std::endl;
					for (uint8_t i = 0; i < level; ++i)
						ss << "  ";
					ss << (left ? "[-] " : "[+] ") << cur->prefix << " " << cur->data;
				}, nullptr);
		return ss.str();
	}

protected:

	struct node;
	using node_ptr_t = std::unique_ptr<node>;

	inline uint8_t len(const iptools::cidr_v4 addr) { return addr.is_net() ? addr.mask() : 32; }

	struct node
	{
		node(uint8_t len, uint32_t prefix, T data)
			: len(len > 32 ? 32 : len)
			, prefix(prefix)
			, data(data)
			, left(nullptr)
			, right(nullptr)
		{}

		node(const iptools::cidr_v4& addr, T data)
			: len(addr.is_net() ? addr.mask() : (uint8_t)32)
			, prefix(addr)
			, data(data)
			, left(nullptr)
			, right(nullptr)
		{}

		template <typename I> void swap(I& x, I& y)
		{
			x = x ^ y;
			y = y ^ x;
			x = x ^ y;
		}

		void swap(node_ptr_t& rhv)
		{
			swap(len, rhv->len);
			swap(prefix, rhv->prefix);
			T tmp = data;
			data = rhv->data;
			rhv->data = tmp;
		}

		void swap(iptools::cidr_v4& addr, T& data)
		{
			T aux = this->data;
			this->data = data;
			data = aux;

			iptools::cidr_v4 aux_addr(prefix, len);
			len = addr.is_net() ? addr.mask() : 32;
			prefix = addr;
			addr = aux_addr;
		}

		uint8_t     len;
		uint32_t    prefix;
		T           data;
		node_ptr_t  left;
		node_ptr_t  right;
	};

	void insert(iptools::cidr_v4 addr, T data, node_ptr_t& cur, uint8_t level)
	{
		if (len(addr) > cur->len)
			cur->swap(addr, data);
		if (len(addr) == level)
			return;
		if ((((uint32_t)addr >> (31 - level)) & 1) == 0)
		{
			if (!cur->left)
				cur->left.reset(new node(addr, data));
			else
				insert(addr, data, cur->left, level + 1);
		}
		else
		{
			if (!cur->right)
				cur->right.reset(new node(addr, data));
			else
				insert(addr, data, cur->right, level + 1);
		}
	}

	void remove(const iptools::cidr_v4& toremove, node_ptr_t& cur, uint8_t level)
	{
		if (!cur)
			return;
		if (toremove == cur->prefix)
		{
			if (!cur->right && !cur->left)
			{
				cur.reset(nullptr);
				return;
			}
			if (!cur->right || (cur->left && (cur->left->len > cur->right->len)))
			{
				cur->swap(cur->left);
				remove(toremove, cur->left, level+1);
			}
			else
			{
				cur->swap(cur->right);
				remove(toremove, cur->right, level+1);
			}
			return;
		}
		if (len(toremove) == level)
			return;
		if ((((uint32_t)toremove >> (31 - level)) & 1) == 0)
			remove(toremove, cur->left, level+1);
		else
			remove(toremove, cur->right, level+1);
	}

	void walk(node_ptr_t& cur,
	          int8_t level,
	          std::function<void(node_ptr_t& node, uint8_t level, bool left)> fun_before,
	          std::function<void(node_ptr_t& node, uint8_t level)> fun_after)
	{
		if (!cur)
			 return;
		if (cur->left)
		{
			if (fun_before)
				fun_before(cur->left, level+1, true);
			walk(cur->left, level+1, fun_before, fun_after);
		}
		if (cur->right)
		{
			if (fun_before)
				fun_before(cur->right, level+1, false);
			walk(cur->right, level+1, fun_before, fun_after);
		}
		if (fun_after)
			fun_after(cur, level);
	}

private:

	void recurse_copy(const node_ptr_t& from, node_ptr_t& to)
	{
		if (from->right)
		{
			to->right.reset(new node(from->right->len, from->right->prefix, from->right->data));
			recurse_copy(from->right, to->right);
		}
		if (from->left)
		{
			to->left.reset(new node(from->left->len, from->left->prefix, from->left->data));
			recurse_copy(from->left, to->left);
		}
	}

	node_ptr_t root{nullptr};
	size_t  size{0};
};

// preserve back compatibility
class lpfst : public basic_lpfst<void*>
{
public:
	lpfst() : basic_lpfst() {}

	lpfst(const lpfst& copy) : basic_lpfst(copy) { }

	lpfst& operator=(const lpfst& copy)
	{
		basic_lpfst<void*>::operator=((basic_lpfst<void*>)copy);
		return *this;
	}

	void insert(iptools::cidr_v4 addr)
	{
		basic_lpfst<void*>::insert(addr, nullptr);
	}

	bool check(const iptools::cidr_v4& addr) const
	{
		void* ptr;
		return basic_lpfst<void*>::check(addr, ptr);
	}

	bool check(const uint32_t addr) const
	{
		void* ptr;
		return basic_lpfst<void*>::check(addr, ptr);
	}
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


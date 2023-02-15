/**@author hoxnox <hoxnox@gmail.com>
 * @date 20320215 16:02:45*/

#pragma once
#include "cidr.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <functional>

namespace iptools {

using in6_addr_t = std::array<uint8_t, 16>;

/**@brief Data structure allows to add some CIDR networks and check if
 * the given address belongs to any of them.
 *
 * Based on Longest Prefix First Search Tree (LPFST)*/
template <class T> class basic_lpfst_v6
{
public:
	basic_lpfst_v6()
		: root_{nullptr}
		, size_(0)
	{}

	virtual ~basic_lpfst_v6()
	{
		clear();
	}

	basic_lpfst_v6(const basic_lpfst_v6& copy)
	{
		operator=(copy);
	}

	basic_lpfst_v6& operator=(const basic_lpfst_v6& copy)
	{
		clear();
		if (!copy.root_)
			return *this;
		root_.reset(new node(copy.root_->len, copy.root_->prefix, copy.root_->data));
		recurse_copy(copy.root_, root_);
		size_ = copy.size_;
		return *this;
	}

	size_t size() const
	{
		return size_;
	}

	void insert(iptools::cidr_v6 addr, T data)
	{
		if (!root_)
		{
			root_.reset(new node(addr, data));
			size_ = 1;
			return;
		}
		insert(addr, data, root_, 0);
	}

	void remove(const iptools::cidr_v6& addr)
	{
		remove(addr, root_, 0);
		--size_;
	}

	/**@return true if the address belongs any of the inserted CIDRs*/
	bool check(const iptools::cidr_v6& addr, T& data) const
	{
		node*    y      = root_.get();
		uint8_t  level  = 0;
		bool     is_net = addr.is_net();
		uint8_t  mask   = addr.mask();
		while (y != nullptr)
		{
			if (is_net && mask < level)
				return true;
			if (!is_net || (is_net && mask >= y->len))
			{
				if (addr.has_prefix(y->prefix, y->len))
				{
					data = y->data;
					return true;
				}
			}
			if (!addr.check_bit(127-level))
				y = y->left.get();
			else
				y = y->right.get();
			++level;
		}
		return false;
	}

	/**@return true if the address belongs any of the inserted CIDRs
	 * @param addr in host byte order*/
	bool check(const in6_addr_t addr, T& data) const
	{
		node*    y     = root_.get();
		uint8_t  level = 0;
		while (y != nullptr)
		{
			if (has_prefix(addr, y->prefix, y->len))
			{
				data = y->data;
				return true;
			}
			if (!check_bit(addr, 127-level))
				y = y->left.get();
			else
				y = y->right.get();
			++level;
		}
		return false;
	}

	bool empty() const
	{
		return !(bool)root_;
	}

	void clear()
	{
		walk(root_, 0, nullptr, [](node_ptr_t& cur, uint8_t level) {
			cur->left.reset(nullptr);
			cur->right.reset(nullptr);
		});
		root_.reset(nullptr);
		size_ = 0;
	}

	std::string print()
	{
		if (!root_)
			return {};
		std::stringstream ss;
		ss << root_->prefix << "/" << (int)root_->len << " " << root_->data;
		walk(
			root_, 0,
			[&ss](node_ptr_t& cur, uint8_t level, bool left) {
				ss << std::endl << (int)level;
				for (uint8_t i = 0; i < level; ++i)
					ss << "  ";
				ss << (left ? "[-] " : "[+] ") << cur->prefix << "/" << (int)cur->len << " " << cur->data;
			},
			nullptr);
		return ss.str();
	}

protected:
	struct node;
	using node_ptr_t = std::unique_ptr<node>;

	inline uint8_t len(const iptools::cidr_v6 addr)
	{
		return addr.is_net() ? addr.mask() : 128;
	}

	struct node
	{
		node(uint8_t len, in6_addr_t prefix, T data)
			: len(len > 128 ? 128 : len)
			, prefix(prefix)
			, data(data)
			, left(nullptr)
			, right(nullptr)
		{}

		node(const iptools::cidr_v6& addr, T data)
			: len(addr.is_net() ? addr.mask() : (uint8_t)128)
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
			prefix.swap(rhv->prefix);
			T tmp     = data;
			data      = rhv->data;
			rhv->data = tmp;
		}

		void swap(iptools::cidr_v6& addr, T& data)
		{
			T aux      = this->data;
			this->data = data;
			data       = aux;

			iptools::cidr_v6 aux_addr(prefix, len);
			len    = addr.is_net() ? addr.mask() : 128;
			prefix = addr;
			addr   = aux_addr;
		}

		uint8_t                  len;
		in6_addr_t               prefix;
		T                        data;
		node_ptr_t               left;
		node_ptr_t               right;
	};

	void insert(iptools::cidr_v6 addr, T data, node_ptr_t& cur, uint8_t level)
	{
		if (len(addr) >= cur->len)
			cur->swap(addr, data);
		if (len(addr) == cur->len && addr.has_prefix(cur->prefix, cur->len))
			return;
		if (len(addr) == level)
			return;
		if (!addr.check_bit(127-level))
		{
			if (!cur->left)
			{
				cur->left.reset(new node(addr, data));
				++size_;
			}
			else
			{
				insert(addr, data, cur->left, level + 1);
			}
		}
		else
		{
			if (!cur->right)
			{
				cur->right.reset(new node(addr, data));
				++size_;
			}
			else
			{
				insert(addr, data, cur->right, level + 1);
			}
		}
	}

	void remove(const iptools::cidr_v6& toremove, node_ptr_t& cur, uint8_t level)
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
				remove(toremove, cur->left, level + 1);
			}
			else
			{
				cur->swap(cur->right);
				remove(toremove, cur->right, level + 1);
			}
			return;
		}
		if (len(toremove) == level)
			return;
		if (!toremove.check_bit(127-level))
			remove(toremove, cur->left, level + 1);
		else
			remove(toremove, cur->right, level + 1);
	}

	void walk(node_ptr_t&                                               cur,
		int8_t                                                          level,
		std::function<void(node_ptr_t& node, uint8_t level, bool left)> fun_before,
		std::function<void(node_ptr_t& node, uint8_t level)>            fun_after)
	{
		if (!cur)
			return;
		if (cur->left)
		{
			if (fun_before)
				fun_before(cur->left, level + 1, true);
			walk(cur->left, level + 1, fun_before, fun_after);
		}
		if (cur->right)
		{
			if (fun_before)
				fun_before(cur->right, level + 1, false);
			walk(cur->right, level + 1, fun_before, fun_after);
		}
		if (fun_after)
			fun_after(cur, level);
	}

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

	node_ptr_t root_{nullptr};
	size_t     size_{0};
};

// preserve back compatibility
class lpfst_v6 : public basic_lpfst_v6<void*>
{
public:
	lpfst_v6()
		: basic_lpfst_v6()
	{}

	lpfst_v6(const lpfst_v6& copy)
		: basic_lpfst_v6(copy)
	{}

	lpfst_v6& operator=(const lpfst_v6& copy)
	{
		basic_lpfst_v6<void*>::operator=((basic_lpfst_v6<void*>)copy);
		return *this;
	}

	void insert(iptools::cidr_v6 addr)
	{
		basic_lpfst_v6<void*>::insert(addr, nullptr);
	}

	bool check(const iptools::cidr_v6& addr) const
	{
		void* ptr;
		return basic_lpfst_v6<void*>::check(addr, ptr);
	}

	bool check(const in6_addr_t addr) const
	{
		void* ptr;
		return basic_lpfst_v6<void*>::check(addr, ptr);
	}
};

inline lpfst_v6
internet_blacklist_v6()
{
	lpfst_v6 result;
	// RFC-1918: Private-Use
	result.insert({"fc00::/8"});
	return result;
}

} // namespace iptools

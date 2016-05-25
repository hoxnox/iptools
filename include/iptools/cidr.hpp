/**@author hoxnox <hoxnox@gmail.com>
 * @date 20160405 11:39:23 */

#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>
#include <arpa/inet.h>

namespace iptools {

class cidr_v4
{
public:
	cidr_v4() {}
	cidr_v4(std::string str);
	cidr_v4(uint32_t addr, uint8_t mask) : addr_(addr), mask_(32-mask) {}
	~cidr_v4() = default;

	bool operator==(const cidr_v4& rhv) const;
	bool operator!=(const cidr_v4& rhv) const;
	operator uint32_t() const { return addr_; }
	uint32_t mask() const { return 32-mask_; }

	/**@brief get first address (network address) in the network*/
	uint32_t    first() const { return (addr_>>mask_)<<mask_; }
	/**@brief get last address in the network*/
	uint32_t    last()  const;
	/**@brief check if address is int the given cidr*/
	bool        in(const cidr_v4& net) const;
	/**@brief check if this cidr is network address*/
	bool        is_net() const { return (addr_&(0xFFFFFFFF>>(32-mask_))) == 0; }
	/**@brief get network address*/
	cidr_v4     net() const { return cidr_v4((addr_>>mask_)<<mask_, 32-mask_); }
	/**@brief convert to string*/
	std::string str(bool nomask = false) const;

	class const_iterator;
	const_iterator     begin() const;
	const_iterator     end() const;

private:
	uint32_t addr_{0};
	uint8_t  mask_{0}; //!< @warning it's inverted mask
};

class cidr_v4::const_iterator
	: public std::iterator<std::random_access_iterator_tag, cidr_v4, uint64_t>
{
public:
	const_iterator() {}
	const_iterator(const const_iterator& copy) = default;
	const_iterator&  operator=(const const_iterator& copy) = default;
	bool             operator==(const const_iterator& rhv) const;
	bool             operator!=(const const_iterator& rhv) const;
	const_iterator   operator++(int);
	const_iterator&  operator++();
	const_iterator&  operator+=(uint32_t n);
	const_iterator   operator+(uint32_t n);
	cidr_v4          operator*() const;
	uint64_t         distance(const const_iterator& rhv) const;
private:
	uint32_t pos_;
	uint32_t end_;
	uint8_t  mask_; //!< @warning - it's normal mask
	static const int8_t END_FLAG = 1<<7;
friend class cidr_v4;
};

} // namespace

namespace std
{
	inline ostream& operator<<(ostream& strm, const iptools::cidr_v4& cidr)
	{
		strm << cidr.str();
		return strm;
	}
	inline ostream& operator<<(ostream& strm, const iptools::cidr_v4::const_iterator& i)
	{
		strm << (*i).str();
		return strm;
	}
	inline typename iterator_traits<iptools::cidr_v4::const_iterator>::difference_type
	distance (const iptools::cidr_v4::const_iterator& first,
	          const iptools::cidr_v4::const_iterator& last)
	{
		return first.distance(last);
	}
} // namespace



////////////////////////////////////////////////////////////////////////
// inline

namespace iptools {

inline
cidr_v4::cidr_v4(std::string str)
	: addr_(0)
	, mask_(0)
{
	std::string addr = str;
	const auto f = str.find('/');
	if (f == std::string::npos)
	{
		mask_ = 0;
	}
	else
	{
		addr = str.substr(0, f);
		std::string mask = str.substr(f + 1, str.length() - f - 1);
		if (addr.empty() || addr.size() < 7 || mask.empty() || mask.size() > 3)
			return;
		mask_ = static_cast<uint8_t>(std::atoi(mask.c_str()));
		if (mask_ > 32)
		{
			mask_ = 0;
			return;
		}
		mask_ = 32 - mask_;
	}
	if (inet_pton(AF_INET, addr.c_str(), &addr_) != 1)
	{
		addr_ = 0;
		return;
	}
	addr_ = ntohl(addr_);
}

inline std::string
cidr_v4::str(bool nomask) const
{
	char tmp[20];
	uint32_t addr = htonl(addr_);
	if (inet_ntop(AF_INET, &addr, tmp, sizeof(tmp)) == NULL)
		return std::string();
	if (nomask)
		return tmp;
	std::stringstream ss;
	ss << tmp << "/" << 32-mask_;
	return ss.str();
}

inline bool
cidr_v4::in(const cidr_v4& net) const
{
	if (!net.is_net())
		return operator==(net);
	if (is_net())
	{
		return mask_ > net.mask_ ?
			false : ((addr_>>net.mask_)==(net.addr_>>net.mask_));
	}
	return (net.addr_>>mask_)==(addr_>>mask_);
}

inline cidr_v4::const_iterator
cidr_v4::begin() const
{
	const_iterator begin;
	if (!is_net())
	{
		begin.pos_ = addr_;
		begin.end_ = addr_;
	}
	else
	{
		begin.pos_ = first() + 1;
		begin.end_ = last();
	}
	begin.mask_ = 32-mask_;
	return begin;
}

inline cidr_v4::const_iterator
cidr_v4::end() const
{
	const_iterator end;
	if (!is_net())
	{
		end.pos_ = addr_;
		end.end_ = addr_;
	}
	else
	{
		end.pos_ = last();
		end.end_ = last();
	}
	end.mask_ = 32-mask_;
	end.mask_ |= const_iterator::END_FLAG;
	return end;
}

inline bool
cidr_v4::operator!=(const cidr_v4& rhv) const
{
	return mask_ != rhv.mask_ || addr_ != rhv.addr_;
}

inline bool
cidr_v4::operator==(const cidr_v4& rhv) const
{
	return mask_ == rhv.mask_ && addr_ == rhv.addr_;
}

inline uint32_t
cidr_v4::last() const
{
	return mask_ == 0 ?
		addr_ : (((addr_>>mask_)<<mask_)|(0xFFFFFFFF>>(32-mask_)));
}

inline bool
cidr_v4::const_iterator::operator==(const const_iterator& rhv) const
{
	return pos_ == rhv.pos_ && end_ == rhv.end_ && mask_ == rhv.mask_;
}

inline bool
cidr_v4::const_iterator::operator!=(const const_iterator& rhv) const
{
	return !operator==(rhv);
}

inline cidr_v4::const_iterator
cidr_v4::const_iterator::operator++(int)
{
	const_iterator cur = *this;
	operator++();
	return cur;
}

inline cidr_v4::const_iterator&
cidr_v4::const_iterator::operator++()
{
	if (pos_ < end_)
		++pos_;
	else
		mask_ |= END_FLAG;
	return *this;
}

inline cidr_v4::const_iterator&
cidr_v4::const_iterator::operator+=(uint32_t n)
{
	return *this = *this + n;
}

inline cidr_v4::const_iterator
cidr_v4::const_iterator::operator+(uint32_t n)
{
	const_iterator rs(*this);
	if (rs.end_ - rs.pos_ >= n)
	{
		rs.pos_ += n;
	}
	else
	{
		rs.pos_ = rs.end_;
		rs.mask_ |= END_FLAG;
	}
	return rs;
}

inline cidr_v4
cidr_v4::const_iterator::operator*() const
{
	return cidr_v4(pos_, mask_);
}

inline uint64_t
cidr_v4::const_iterator::distance(const const_iterator& rhv) const
{
	if (end_ != rhv.end_ || (mask_&127)!=(rhv.mask_&127))
		return 0;
	if (pos_ >= rhv.pos_)
	{
		if(!(mask_ & END_FLAG))
			return 1;
		else
			return 0;
	}
	return static_cast<uint64_t>(rhv.pos_) - pos_ + 1;
}

} // namespace


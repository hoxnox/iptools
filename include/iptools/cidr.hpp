/**@author hoxnox <hoxnox@gmail.com>
 * @date 20160405 11:39:23 */

#pragma once

#include <cstdint>
#include <string>
#include <algorithm>
#include <memory>
#include <arpa/inet.h>

namespace rators {

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

private:
	uint32_t addr_{0};
	uint8_t  mask_{0}; //!< @warning it's inverted mask
};

} // namespace

namespace std
{
	inline ostream& operator<<(ostream& strm, const rators::cidr_v4& cidr)
	{
		strm << cidr.str();
		return strm;
	}
} // namespace


////////////////////////////////////////////////////////////////////////
// inline

namespace rators {

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

} // namespace

/**@author hoxnox <hoxnox@gmail.com>
 * @date $date$ */

#pragma once

#include <string>
#include <iostream>
#include <array>

namespace iptools {

class cidr_v6
{
public:
	cidr_v6() {}
	cidr_v6(std::string str);
	/**@param addr should be in host byte order*/
	cidr_v6(std::array<uint8_t, 16> addr, uint8_t mask) : addr_(addr), mask_(mask) {}
	~cidr_v6() = default;

	bool operator==(const cidr_v6& rhv) const;
	bool operator!=(const cidr_v6& rhv) const;
	operator std::array<uint8_t, 16>() const { return addr_; }
	uint32_t mask() const { return mask_; }

	/**@brief get first address (network address in host byte order) in the network*/
    std::array<uint8_t, 16> first() const;
	/**@brief get last address in the network (host byte order)*/
    std::array<uint8_t, 16> last()  const;
	/**@brief check if address is in the given cidr*/
	bool        in(const cidr_v6& net) const;
	/**@brief check if this cidr is network address*/
	bool        is_net() const;
	/**@brief get network address*/
	cidr_v6     net() const;
	/**@brief convert to string*/
	std::string str(bool nomask = false) const;

private:
	std::array<uint8_t, 16> addr_{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
	uint8_t  mask_{0}; //!< @warning it's normal mask
};

} // namespace

namespace std
{
	inline ostream& operator<<(ostream& strm, const iptools::cidr_v6& cidr)
	{
		strm << cidr.str();
		return strm;
	}
} // namespace

////////////////////////////////////////////////////////////////////////
// inline

namespace iptools {

inline
cidr_v6::cidr_v6(std::string str)
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
		if (addr.empty() || mask.empty() || mask.size() > 4)
			return;
		mask_ = static_cast<uint8_t>(std::atoi(mask.c_str()));
		if (mask_ > 128)
		{
			mask_ = 0;
			return;
		}
	}
	if (inet_pton(AF_INET6, addr.c_str(), &addr_) != 1)
	{
		addr_ = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
		return;
	}
}

inline std::string
cidr_v6::str(bool nomask) const
{
	char tmp[50];
	if (inet_ntop(AF_INET6, &addr_, tmp, sizeof(tmp)) == NULL)
		return std::string();
	if (nomask)
		return tmp;
	std::stringstream ss;
	ss << tmp << "/" << (int)mask_;
	return ss.str();
}

inline bool
cidr_v6::is_net() const
{
	uint8_t bytes = (128-mask_)/8;
	uint8_t bits  = (128-mask_)%8;
	uint8_t i = 0;
	for (; i < bytes; ++i)
	{
		if (addr_[15-i] != 0)
			return false;
	}
    if ((addr_[15-i]&(0xFF>>(8-bits))) == 0)
		return true;
	return false;
}

inline bool
cidr_v6::in(const cidr_v6& net) const
{
	if (!net.is_net())
		return operator==(net);
	if (mask_ < net.mask_)
		return false;
	uint8_t bytes = net.mask_/8;
	uint8_t bits = net.mask_%8;
	uint8_t i = 0;
	for (; i < bytes; ++i)
	{
		if (net.addr_[i] != addr_[i])
			return false;
	}
	if (bits > 0)
	{
		uint8_t net_netaddr = addr_[i]>>(8-bits);
		uint8_t my_netaddr  = net.addr_[i]>>(8-bits);
		if (net_netaddr != my_netaddr)
			return false;
	}
	return true;
}

inline bool
cidr_v6::operator!=(const cidr_v6& rhv) const
{
	return mask_ != rhv.mask_ || addr_ != rhv.addr_;
}

inline bool
cidr_v6::operator==(const cidr_v6& rhv) const
{
	return mask_ == rhv.mask_ && addr_ == rhv.addr_;
}

inline cidr_v6
cidr_v6::net() const
{
	auto addr = addr_;
	if (mask_ == 0)
		return {addr, mask_};
	uint8_t bytes = (128-mask_)/8;
	uint8_t bits  = (128-mask_)%8;
	uint8_t i = 0;
	for (; i < bytes; ++i)
		addr[15-i] = 0;
	addr[15-i] = ((addr_[15-i]>>bits)<<bits);
	return {addr, mask_};
}

inline std::array<uint8_t, 16>
cidr_v6::first() const
{
	auto addr = addr_;
	if (mask_ == 0)
		return addr;
	uint8_t bytes = (128-mask_)/8;
	uint8_t bits  = (128-mask_)%8;
	uint8_t i = 0;
	for (; i < bytes; ++i)
		addr[15-i] = 0;
	addr[15-i] = ((addr_[15-i]>>bits)<<bits);
	addr[15] |= 1;
	return addr;
}

inline std::array<uint8_t, 16>
cidr_v6::last() const
{
	auto addr = addr_;
	if (mask_ == 0)
		return addr;
	uint8_t bytes = (128-mask_)/8;
	uint8_t bits  = (128-mask_)%8;
	uint8_t i = 0;
	for (; i < bytes; ++i)
		addr[15-i] = 0xFF;
	addr[15-i] = ((addr_[15-i]>>bits)<<bits);
	addr[15] |= (0xFF>>(8-bits));
	return addr;
}

} // namespace


/**@author hoxnox <hoxnox@gmail.com>
 * @date $date$ */

#pragma once

#include <string>
#include <iostream>
#include <array>

namespace iptools {

bool
	check_bit(const std::array<uint8_t, 16>& num, uint8_t bitno);

std::array<uint8_t, 16>
	set_bit(const std::array<uint8_t, 16>& num, uint8_t bitno, bool value);

std::string
	print_binary(const std::array<uint8_t, 16>& n, uint8_t mask);

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
	std::string bstr() const { return print_binary(addr_, mask_); }
	/**@brief check n'th bit, if 1 returns true, false otherwise*/
	bool check_bit(uint8_t bitno) const;
	/**@brief check first n bits to be the same sa in prefix*/
	bool has_prefix(std::array<uint8_t, 16> prefix, uint8_t len) const;

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

	inline ostream& operator<<(ostream& strm, const std::array<uint8_t, 16>& addr)
	{
		char tmp[50];
		if (inet_ntop(AF_INET6, addr.data(), tmp, sizeof(tmp)) != NULL)
			strm << tmp;
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
		mask_ = 128;
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
			mask_ = 128;
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

inline bool
check_bit(const std::array<uint8_t, 16>& num, uint8_t bitno)
{
	if (bitno >= 128)
		return false;
	return (num[15-bitno/8]>>bitno%8)&1 == 1;
}

inline std::array<uint8_t, 16>
set_bit(const std::array<uint8_t, 16>& num, uint8_t bitno, bool value)
{
	if (bitno >= 128)
		return {};
	auto rs = num;
	if (value)
		rs[15-bitno/8] |= (1<<bitno%8);
	else
		rs[15-bitno/8] &= ~(1<<bitno%8);
	return rs;
}

inline std::string
print_binary(const std::array<uint8_t, 16>& n, uint8_t mask)
{
	char rs[129+15+2];
	rs[sizeof(rs)-1] = 0;
	rs[0] = '[';
	if (mask == 0)
		rs[1] = ']';
	else
		rs[mask+mask/8+(mask%8==0?0:1)] = ']';
	size_t pos = 1;
	for (size_t i = 0; i < 128; ++i)
	{
		if (i == mask)
			pos++;
		if (i%8 == 0 && i > 0)
			rs[pos++] = '\'';
		rs[pos++] = ((n[i/8]>>(7-i%8))&1) == 1 ? '1' : '0';
	}
	return rs;
}

inline bool
cidr_v6::check_bit(uint8_t bitno) const
{
	return ::check_bit(addr_, bitno);
}

inline bool
has_prefix(const std::array<uint8_t, 16>& addr, const std::array<uint8_t, 16>& prefix, uint8_t len)
{
	if (len == 0)
		len = 1;
	if (len > 128)
		len = 128;
	uint8_t i = 0;
	for (; i < len/8; i++)
		if (addr[i] != prefix[i])
			return false;
	uint8_t shift = 8-len%8;
	return addr[i]>>shift == prefix[i]>>shift;
}

inline bool
cidr_v6::has_prefix(std::array<uint8_t, 16> prefix, uint8_t len) const
{
	return ::has_prefix(addr_, prefix, len);
}

} // namespace


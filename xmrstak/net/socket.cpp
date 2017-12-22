/*
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  * Additional permission under GNU GPL version 3 section 7
  *
  * If you modify this Program, or any covered work, by linking or combining
  * it with OpenSSL (or a modified version of that library), containing parts
  * covered by the terms of OpenSSL License and SSLeay License, the licensors
  * of this Program grant you additional permission to convey the resulting work.
  *
  */

#include "socket.hpp"
#include "jpsock.hpp"
#include "xmrstak/jconf.hpp"
#include "xmrstak/misc/console.hpp"
#include "xmrstak/misc/executor.hpp"

plain_socket::plain_socket(jpsock* err_callback) : pCallback(err_callback)
{
	hSocket = INVALID_SOCKET;
	pSockAddr = nullptr;
}

bool plain_socket::set_hostname(const char* sAddr)
{
	char sAddrMb[256];
	char *sTmp, *sPort;

	size_t ln = strlen(sAddr);
	if (ln >= sizeof(sAddrMb))
		return pCallback->set_socket_error("CONNECT error: Pool address overflow.");

	memcpy(sAddrMb, sAddr, ln);
	sAddrMb[ln] = '\0';

	if ((sTmp = strstr(sAddrMb, "//")) != nullptr)
	{
		sTmp += 2;
		memmove(sAddrMb, sTmp, strlen(sTmp) + 1);
	}

	if ((sPort = strchr(sAddrMb, ':')) == nullptr)
		return pCallback->set_socket_error("CONNECT error: Pool port number not specified, please use format <hostname>:<port>.");

	sPort[0] = '\0';
	sPort++;

	addrinfo hints = { 0 };
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	pAddrRoot = nullptr;
	int err;
	if ((err = getaddrinfo(sAddrMb, sPort, &hints, &pAddrRoot)) != 0)
		return pCallback->set_socket_error_strerr("CONNECT error: GetAddrInfo: ", err);

	addrinfo *ptr = pAddrRoot;
	std::vector<addrinfo*> ipv4;
	std::vector<addrinfo*> ipv6;

	while (ptr != nullptr)
	{
		if (ptr->ai_family == AF_INET)
			ipv4.push_back(ptr);
		if (ptr->ai_family == AF_INET6)
			ipv6.push_back(ptr);
		ptr = ptr->ai_next;
	}

	if (ipv4.empty() && ipv6.empty())
	{
		freeaddrinfo(pAddrRoot);
		pAddrRoot = nullptr;
		return pCallback->set_socket_error("CONNECT error: I found some DNS records but no IPv4 or IPv6 addresses.");
	}
	else if (!ipv4.empty() && ipv6.empty())
		pSockAddr = ipv4[rand() % ipv4.size()];
	else if (ipv4.empty() && !ipv6.empty())
		pSockAddr = ipv6[rand() % ipv6.size()];
	else if (!ipv4.empty() && !ipv6.empty())
	{
		if(jconf::inst()->PreferIpv4())
			pSockAddr = ipv4[rand() % ipv4.size()];
		else
			pSockAddr = ipv6[rand() % ipv6.size()];
	}

	hSocket = socket(pSockAddr->ai_family, pSockAddr->ai_socktype, pSockAddr->ai_protocol);

	if (hSocket == INVALID_SOCKET)
	{
		freeaddrinfo(pAddrRoot);
		pAddrRoot = nullptr;
		return pCallback->set_socket_error_strerr("CONNECT error: Socket creation failed ");
	}

	return true;
}

bool plain_socket::connect()
{
	int ret = ::connect(hSocket, pSockAddr->ai_addr, (int)pSockAddr->ai_addrlen);

	freeaddrinfo(pAddrRoot);
	pAddrRoot = nullptr;

	if (ret != 0)
		return pCallback->set_socket_error_strerr("CONNECT error: ");
	else
		return true;
}

int plain_socket::recv(char* buf, unsigned int len)
{
	int ret = ::recv(hSocket, buf, len, 0);

	if(ret == 0)
		pCallback->set_socket_error("RECEIVE error: socket closed");
	if(ret == SOCKET_ERROR || ret < 0)
		pCallback->set_socket_error_strerr("RECEIVE error: ");

	return ret;
}

bool plain_socket::send(const char* buf)
{
	int pos = 0, slen = strlen(buf);
	while (pos != slen)
	{
		int ret = ::send(hSocket, buf + pos, slen - pos, 0);
		if (ret == SOCKET_ERROR)
		{
			pCallback->set_socket_error_strerr("SEND error: ");
			return false;
		}
		else
			pos += ret;
	}

	return true;
}

void plain_socket::close(bool free)
{
	if(hSocket != INVALID_SOCKET)
	{
		sock_close(hSocket);
		hSocket = INVALID_SOCKET;
	}
}

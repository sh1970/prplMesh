/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/network/interface_flags_reader_impl.h>
#include <bcl/network/sockets_impl.h>

#include <net/if.h>
#include <sys/ioctl.h>

namespace beerocks {
namespace net {

bool InterfaceFlagsReaderImpl::read_flags(uint32_t iface_index, uint16_t &iface_flags)
{
    UdpSocket socket;

    ifreq if_req;
    if (0 == if_indextoname(iface_index, if_req.ifr_name)) {
        return false;
    }

    int rc = ioctl(socket.fd(), SIOCGIFFLAGS, &if_req);
    if (rc == -1) {
        return false;
    }

    iface_flags = if_req.ifr_flags;

    return true;
}

} // namespace net
} // namespace beerocks

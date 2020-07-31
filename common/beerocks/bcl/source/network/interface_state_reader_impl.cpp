/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/network/interface_flags_reader.h>
#include <bcl/network/interface_state_reader_impl.h>

#include <net/if.h>

namespace beerocks {
namespace net {

bool InterfaceStateReaderImpl::read_state(uint32_t iface_index, bool &iface_state)
{
    uint16_t flags;
    if (!m_interface_flags_reader->read_flags(iface_index, flags)) {
        return false;
    }

    iface_state = (flags & IFF_UP) && (flags & IFF_RUNNING);

    return true;
}

} // namespace net
} // namespace beerocks

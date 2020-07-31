/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_STATE_READER_H_
#define BCL_NETWORK_INTERFACE_STATE_READER_H_

#include <stdint.h>

namespace beerocks {
namespace net {

class InterfaceStateReader {
public:
    virtual ~InterfaceStateReader() = default;

    /**
     * @brief Reads interface up-and-running state.
     *
     * Reads the state of the network interface with given index. State is true if the interface is
     * up and running and false otherwise.
     *
     * @param[in] iface_index Interface index.
     * @param[out] iface_state Interface state (true if it is up and running).
     * @return True on success and false otherwise.
     */
    virtual bool read_state(uint32_t iface_index, bool &iface_state) = 0;
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_STATE_READER_H_ */

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_FLAGS_READER_H_
#define BCL_NETWORK_INTERFACE_FLAGS_READER_H_

#include <stdint.h>

namespace beerocks {
namespace net {

class InterfaceFlagsReader {
public:
    virtual ~InterfaceFlagsReader() = default;

    /**
     * @brief Reads interface flags.
     *
     * Reads the active flag word of the network interface with given index.
     *
     * @param[in] iface_index Interface index.
     * @param[out] iface_flags Device flags (bitmask)
     * @return True on success and false otherwise.
     */
    virtual bool read_flags(uint32_t iface_index, uint16_t &iface_flags) = 0;
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_FLAGS_READER_H_ */

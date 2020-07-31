/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_STATE_MONITOR_H_
#define BCL_NETWORK_INTERFACE_STATE_MONITOR_H_

#include <stdint.h>

#include <functional>

namespace beerocks {
namespace net {

class InterfaceStateMonitor {
public:
    /**
     * Network interface state-change handler function.
     */
    using StateChangeHandler = std::function<void(uint32_t iface_index, bool iface_state)>;

    /**
     * @brief Class destructor
     */
    virtual ~InterfaceStateMonitor() = default;

    /**
     * @brief Starts the interface state monitor.
     *
     * Starts monitoring the state of the network interface and calls back the installed handler
     * (if any) whenever the interface state changes to or from the up-and-running value.
     *
     * @return True on success and false otherwise.
     */
    virtual bool start() = 0;

    /**
     * @brief Stops the interface state monitor.
     *
     * @return True on success and false otherwise.
     */
    virtual bool stop() = 0;

    /**
     * @brief Sets the state change handler function.
     *
     * Sets the callback function to handle network interface state changes.
     * Use nullptr to remove previously installed callback function.
     *
     * @param handler State change handler function (or nullptr).
     */
    void set_handler(const StateChangeHandler &handler) { m_handler = handler; }

protected:
    /**
     * @brief Notifies a network interface state change.
     *
     * @param iface_index Index of the network interface that changed state.
     * @param iface_state New state of the network interface (true means up-and-running).
     */
    void notify_state_changed(uint32_t iface_index, bool iface_state) const
    {
        if (m_handler) {
            m_handler(iface_index, iface_state);
        }
    }

private:
    /**
     * Network interface state-change handler function that is called back whenever any network
     * interface changes its state.
     */
    StateChangeHandler m_handler;
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_STATE_MONITOR_H_ */

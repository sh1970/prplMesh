/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_backport.h>
#include <bcl/beerocks_defines.h>

#include <mapf/common/config.h>

#include "ieee1905_transport.h"

#include <unistd.h>

namespace beerocks {
namespace transport {

using broker::BrokerServer;

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Number of concurrent connections on the server socket
static constexpr int listen_buffer_size = 10;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Ieee1905Transport::Ieee1905Transport(
    const std::shared_ptr<beerocks::net::InterfaceStateMonitor> &interface_state_monitor,
    const std::shared_ptr<EventLoop> &event_loop)
    : m_interface_state_monitor(interface_state_monitor), m_event_loop(event_loop)
{
}

bool Ieee1905Transport::start()
{
    if (m_broker) {
        LOG(ERROR) << "Failed to start 1905 transport: already started";
        return false;
    }

    // Broker server UDS socket
    auto server_socket = std::make_shared<SocketServer>(
        std::string(TMP_PATH "/" BEEROCKS_BROKER_UDS), listen_buffer_size);

    LOG(INFO) << "Starting 1905 transport...";

    // Create the broker server
    m_broker = std::make_unique<BrokerServer>(server_socket, m_event_loop);
    LOG_IF(!m_broker, FATAL) << "Failed creating broker server!";

    // Register broker handlers for internal and external messages
    m_broker->register_external_message_handler(
        [&](std::unique_ptr<messages::Message> &msg, BrokerServer &broker) -> bool {
            LOG(DEBUG) << "Processing external message: " << uint32_t(msg->type());
            handle_broker_pollin_event(msg);
            return true;
        });

    m_broker->register_internal_message_handler(
        [&](std::unique_ptr<messages::Message> &msg, BrokerServer &broker) -> bool {
            LOG(DEBUG) << "Processing internal message: " << uint32_t(msg->type());
            handle_broker_pollin_event(msg);
            return true;
        });

    m_interface_state_monitor->set_handler([&](uint32_t iface_index, bool iface_state) {
        handle_interface_status_change(iface_index, iface_state);
    });

    return true;
}

bool Ieee1905Transport::stop()
{
    if (!m_broker) {
        LOG(ERROR) << "Failed to stop 1905 transport: not started";
        return false;
    }

    m_interface_state_monitor->set_handler(nullptr);

    return true;
}

} // namespace transport
} // namespace beerocks

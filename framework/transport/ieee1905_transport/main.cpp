/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ieee1905_transport.h"

#include <bcl/beerocks_event_loop_impl.h>
#include <bcl/network/interface_state_monitor_impl.h>
#include <bcl/network/sockets_impl.h>

#include <net/if.h>
#include <unistd.h>

using namespace beerocks;
using namespace beerocks::net;
using namespace beerocks::transport;

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("transport");

    /**
     * Create NETLINK_ROUTE netlink socket for kernel/user-space communication.
     */
    auto socket = std::make_shared<NetlinkRouteSocket>();

    /**
     * Create client socket.
     * Bind it to route netlink multicast group to listen for multicast packets sent from the
     * kernel containing network interface create/delete/up/down events.
     */
    ClientSocketImpl<NetlinkRouteSocket> client(socket);
    client.bind(NetlinkAddress(RTMGRP_LINK));

    /**
     * Create connection to send/receive data using this socket
     */
    auto connection = std::make_shared<SocketConnectionImpl>(socket);

    /**
     * Create application event loop to wait for blocking I/O operations.
     */
    auto event_loop = std::make_shared<EventLoopImpl>();

    /**
     * Create the interface state monitor.
     */
    auto interface_state_monitor =
        std::make_shared<InterfaceStateMonitorImpl>(connection, event_loop);

    /**
     * Application exit code: 0 on success and -1 on failure.
     * From this point on, there's a single exit point to allow for start/stop methods to be
     * cleanly called in pairs.
     */
    int exit_code = 0;

    /**
     * Start the interface state monitor
     */
    if (interface_state_monitor->start()) {
        /**
         * Create the IEEE1905 transport process.
         */
        Ieee1905Transport ieee1905_transport(interface_state_monitor, event_loop);

        /**
         * Start the IEEE1905 transport process
         */
        if (ieee1905_transport.start()) {

            // Run the application event loop
            MAPF_INFO("starting main loop...");
            while (0 == exit_code) {
                if (event_loop->run() < 0) {
                    LOG(ERROR) << "Broker event loop failure!";
                    exit_code = -1;
                }
            }
            MAPF_INFO("done");

            ieee1905_transport.stop();

        } else {
            LOG(ERROR) << "Unable to start IEEE1905 transport process!";
            exit_code = -1;
        }

        interface_state_monitor->stop();

    } else {
        LOG(ERROR) << "Unable to start interface state monitor!";
        exit_code = -1;
    }

    return exit_code;
}

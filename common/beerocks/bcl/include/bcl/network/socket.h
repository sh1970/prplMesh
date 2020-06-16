/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef SOCKET_H
#define SOCKET_H

#if __linux
#elif __unix
#elif __posix
#else
#ifndef IS_WINDOWS
#define IS_WINDOWS
#endif
#endif

#ifdef IS_WINDOWS
#include <WinSock2.h>
#include <windows.h>
typedef unsigned __int64 ssize_t;
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

class Socket {
public:
    // explicit Socket(SOCKET s, long readTimeout = 1000);
    // Socket(SOCKET s, const std::string &peer_ip, int port_port, long readTimeout = 1000);
    // Socket(const std::string &uds_path = std::string(), long readTimeout = 1000);
    static Socket *socketFactory(SOCKET s, long readTimeout = 1000);
    virtual ~Socket();

    enum SocketMode { SocketModeBlocking, SocketModeNonBlocking };

    virtual ssize_t getBytesReady()                                      = 0;
    virtual ssize_t readBytes(uint8_t *buf, size_t buf_size, bool blocking, size_t buf_len = 0,
                              bool isPeek = false, bool waitAll = false) = 0;
    /**
     * @brief read and discard specified number of bytes
     *
     * @return error code if an error occured or number of discarded bytes otherwise
     */
    ssize_t discardBytes(size_t bytes_to_discard);
    virtual ssize_t writeBytes(const uint8_t *buf, size_t buf_len, int port = 0,
                               struct sockaddr_in addr_in = {}) = 0;
    virtual ssize_t writeString(std::string msg)
    {
        return writeBytes((const uint8_t *)msg.c_str(), (size_t)msg.length());
    }
    virtual bool setWriteTimeout(long msec) = 0;
    virtual bool setReadTimeout(long msec)  = 0;
    virtual void closeSocket()              = 0;
    virtual bool isOpen()                   = 0;
    virtual SOCKET getSocketFd()            = 0;
    virtual std::string getError()          = 0;
    virtual std::string getPeerIP()         = 0;
    virtual int getPeerPort()               = 0;
    virtual std::string getUdsPath()        = 0;
    virtual void setPeerMac(const std::string &mac) = 0;
    virtual std::string getPeerMac() =0;

    virtual bool isAcceptedSocket() = 0;
    virtual void setIsServer() = 0;

#ifndef IS_WINDOWS
    virtual size_t getBytesWritePending() = 0;
#endif
protected:
    friend class SocketServer;
    friend class SocketSelect;
    virtual void setAcceptedSocket(bool) = 0;
    virtual SOCKET getSocket() const = 0;
    virtual void setUdsPath(const std::string &) = 0;
/*
    SOCKET m_socket = INVALID_SOCKET;
    std::string m_error;
    std::string m_peer_ip;
    std::string m_uds_path;
    int m_peer_port = 0;
    std::string m_peer_mac;
    bool m_accepted_socket  = false;
    bool m_external_handler = false;
    bool m_is_server        = false;

private:
    static int m_ref;
*/
};

class DefaultSocket : public Socket {
public:
    explicit DefaultSocket(SOCKET s, long readTimeout = 1000);
    DefaultSocket(SOCKET s, const std::string &peer_ip, int port_port, long readTimeout = 1000);
    DefaultSocket(const std::string &uds_path = std::string(), long readTimeout = 1000);
    virtual ~DefaultSocket();

    enum SocketMode { SocketModeBlocking, SocketModeNonBlocking };

    ssize_t getBytesReady();
    ssize_t readBytes(uint8_t *buf, size_t buf_size, bool blocking, size_t buf_len = 0,
                      bool isPeek = false, bool waitAll = false);
    ssize_t writeBytes(const uint8_t *buf, size_t buf_len, int port = 0,
                       struct sockaddr_in addr_in = {});
    bool setWriteTimeout(long msec);
    bool setReadTimeout(long msec);
    void closeSocket();
    bool isOpen();
    SOCKET getSocketFd() { return m_socket; }
    std::string getError() { return m_error; }
    std::string getPeerIP() { return m_peer_ip; }
    int getPeerPort() { return m_peer_port; }
    std::string getUdsPath() { return m_uds_path; }
    void setPeerMac(const std::string &mac) { m_peer_mac = mac; }
    std::string getPeerMac() { return m_peer_mac; }

    bool isAcceptedSocket() { return m_accepted_socket; }
    void setIsServer() { m_is_server = true; }

#ifndef IS_WINDOWS
    size_t getBytesWritePending();
#endif

protected:
    friend class SocketServer;
    friend class SocketSelect;
    SOCKET m_socket = INVALID_SOCKET;
    std::string m_error;
    std::string m_peer_ip;
    std::string m_uds_path;
    int m_peer_port = 0;
    std::string m_peer_mac;
    bool m_accepted_socket  = false;
    bool m_external_handler = false;
    bool m_is_server        = false;
    SOCKET getSocket() const { return m_socket; }
    void setAcceptedSocket(bool value) { m_accepted_socket = value; }
    void setUdsPath(const std::string &value) { m_uds_path = value; }

private:
    static int m_ref;
};

class SocketClient : public DefaultSocket {
public:
    explicit SocketClient(const std::string &uds_path, long readTimeout = 1000);
    SocketClient(const std::string &host, int port, int connect_timeout_msec = -1,
                 long readTimeout = 1000);
};

class SocketServer : public DefaultSocket {
public:
    SocketServer() {}
    SocketServer(const std::string &uds_path, int connections,
                 SocketMode mode = SocketModeBlocking);
    SocketServer(int port, int connections, SocketMode mode = SocketModeBlocking);
    Socket *acceptConnections();
};

class SocketSelect {
public:
    SocketSelect();
    ~SocketSelect();
    void setTimeout(timeval *tval);
    void addSocket(Socket *s);
    void removeSocket(Socket *s);
    void clearReady(Socket *s);
    Socket *at(size_t idx);
    int selectSocket();
    bool readReady(const Socket *s);
    bool readReady(size_t idx);
    int count() { return (int)m_socketVec.size(); }
    bool isBlocking() { return m_isBlocking; }
    std::string getError()
    {
        return m_error;
        m_error.clear();
    }

private:
    bool m_isBlocking;
    std::vector<Socket *> m_socketVec;
    fd_set m_socketSet;
    timeval *m_socketTval;
    std::string m_error;
};

#endif

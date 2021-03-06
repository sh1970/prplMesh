/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_BASE_WLAN_HAL_TYPES_H_
#define _BWL_BASE_WLAN_HAL_TYPES_H_

#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_message_structs.h>
#include <bcl/network/net_struct.h>

#include <stdint.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace bwl {

enum class HALType {
    Invalid = 0,

    AccessPoint,
    Monitor,
    Station
};

enum class IfaceType {
    Unsupported = 0x00,
    Intel       = 0x01,
};

enum class HALState {
    Uninitialized = 0, /*!< Not initialized */
    Initializing  = 1, /*!< Initializing in progress */
    Operational   = 2, /*!< Initialized and active */
    Failed        = 3  /*!< Failed to initialize */
};

enum class AntMode { Invalid = 0, ANT_1X1, ANT_2X2, ANT_3X3, ANT_4X4 };

enum class WiFiChanBW { Invalid = 0, BW_20 = 20, BW_40 = 40, BW_80 = 80 };

struct VAPElement {
    std::string ssid;
    std::string mac;
    bool fronthaul;
    bool backhaul;

    virtual bool operator==(const VAPElement &other) const
    {
        return (ssid == other.ssid && mac == other.mac);
    }

    virtual bool operator!=(const VAPElement &other) const
    {
        return (ssid != other.ssid || mac != other.mac);
    }
};

enum class ChanSwReason { Unknown = 0, Radar = 1, CoEx_20 = 2, CoEx_40 = 3 };

// WLAN Security Types
enum class WiFiSec { Invalid = 0, None, WEP_64, WEP_128, WPA_PSK, WPA2_PSK, WPA_WPA2_PSK };

struct RadioInfo {
    std::string iface_name;
    IfaceType iface_type               = IfaceType::Unsupported;
    bool radio_enabled                 = false;
    int wifi_ctrl_enabled              = 0; // Hostapd / wpa_supplicant
    bool tx_enabled                    = false;
    bool is_5ghz                       = false;
    int channel                        = 0;
    int bandwidth                      = 0;
    int channel_ext_above              = 0;
    int vht_center_freq                = 0;
    bool is_dfs_channel                = false;
    int ant_num                        = 0;
    int tx_power                       = 0;
    beerocks::eFreqType frequency_band = beerocks::eFreqType::FREQ_UNKNOWN; /**< Frequency band */
    beerocks::eWiFiBandwidth max_bandwidth =
        beerocks::eWiFiBandwidth::BANDWIDTH_UNKNOWN; /**< Maximum supported bandwidth */
    bool ht_supported      = false;                  /**< Is HT supported flag */
    uint16_t ht_capability = 0;                      /**< HT capabilities */
    std::basic_string<uint8_t>
        ht_mcs_set; /**< 16-byte attribute containing the MCS set as defined in 802.11n */
    bool vht_supported      = false; /**< Is VHT supported flag */
    uint32_t vht_capability = 0;     /**< VHT capabilities */
    std::basic_string<uint8_t>
        vht_mcs_set; /**< 32-byte attribute containing the MCS set as defined in 802.11ac */
    ChanSwReason last_csa_sw_reason = ChanSwReason::Unknown;
    std::vector<beerocks::message::sWifiChannel> preferred_channels;
    std::vector<beerocks::message::sWifiChannel> supported_channels;
    std::unordered_map<int, VAPElement> available_vaps; // key = vap_id
};

struct hal_conf_t {
    bool ap_acs_enabled = false;
    std::string wpa_ctrl_path;
};

//sta_wlan_hal
typedef struct {
    beerocks::net::sScanResult result;
    char iface_name[beerocks::message::IFACE_NAME_LENGTH];
    uint16_t rx_phy_rate_100kb;
    uint16_t tx_phy_rate_100kb;
    int8_t rx_rssi;
    uint8_t rx_snr;
    int8_t rx_packets;
    uint8_t src_module;
    int8_t vap_id;
} sNodeRssiMeasurement;

typedef struct {
    uint32_t disconnect_reason;
    sMacAddr bssid;
} sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION;

typedef struct {
    sNodeRssiMeasurement params;
} sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE;

//ap_wlan_hal

enum eWiFiBandwidth : uint8_t {
    BANDWIDTH_UNKNOWN = 0,
    BANDWIDTH_20,
    BANDWIDTH_40,
    BANDWIDTH_80,
    BANDWIDTH_80_80,
    BANDWIDTH_160,
    BANDWIDTH_MAX,
};

enum eWiFiStandard : uint8_t {
    STANDARD_NONE = 0x00,
    STANDARD_A    = 0x01,
    STANDARD_B    = 0x02,
    STANDARD_G    = 0x04,
    STANDARD_N    = 0x08,
    STANDARD_AC   = 0x10,
    STANDARD_AX   = 0x20,
};

enum eWiFiMCS : uint8_t {
    MCS_0 = 0,
    MCS_1,
    MCS_2,
    MCS_3,
    MCS_4,
    MCS_5,
    MCS_6,
    MCS_7,
    MCS_8,
    MCS_9,
    MCS_MAX,
};

typedef struct {
    sNodeRssiMeasurement params;
} sACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE;

typedef struct {
    sMacAddr client_mac;
    sMacAddr bssid;
    uint8_t rx_snr;
    uint8_t blocked;   // True if response blocked.
    uint8_t broadcast; // True if broadcast probe.
} sSteeringEvProbeReq;

typedef struct {
    sSteeringEvProbeReq params;
} sACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION;

typedef struct {
    sMacAddr client_mac;
    sMacAddr bssid;
    uint8_t rx_snr;
    uint8_t reason;
    uint8_t blocked; // True if response blocked.
    uint8_t reject;  // True Auth Req is rejected by AP and Auth response is sent to STA with reason
} sSteeringEvAuthFail;

typedef struct {
    sSteeringEvAuthFail params;
} sACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION;

typedef struct {
    sMacAddr mac;
    sMacAddr bssid;
    beerocks::message::sRadioCapabilities capabilities;
    int8_t vap_id;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    size_t association_frame_length;
    uint8_t association_frame[beerocks::message::ASSOCIATION_MAX_LENGTH];
} sClientAssociationParams;

typedef struct {
    sMacAddr mac;
    int8_t vap_id;
    uint8_t reason;
    uint8_t source;
    uint8_t type;
} sClientDisconnectionParams;

typedef struct {
    sClientDisconnectionParams params;
} sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION;

typedef struct {
    sClientAssociationParams params;
} sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION;

typedef struct {
    sMacAddr mac;
    int8_t vap_id;
} sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION;

typedef struct {
    sMacAddr mac;
} sACTION_MONITOR_CLIENT_DISCONNECTED_NOTIFICATION;

typedef struct {
    sMacAddr mac;
    sMacAddr source_bssid;
    sMacAddr target_bssid;
    uint8_t status_code;
} sNodeBssSteerResponse;

typedef struct {
    sNodeBssSteerResponse params;
} sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE;

typedef struct {
    uint32_t timeout;
    uint32_t frequency;
    uint8_t success;
    uint8_t channel;
    uint8_t bandwidth;
    uint8_t reserved1;
} sDfsCacCompleted;

typedef struct {
    sDfsCacCompleted params;
} sACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION;

typedef struct {
    uint32_t frequency;
    uint8_t channel;
    uint8_t bandwidth; //beerocks::eWiFiBandwidth
    uint16_t vht_center_frequency;
} sDfsChannelAvailable;

typedef struct {
    sDfsChannelAvailable params;
} sACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION;

typedef struct {
    int8_t vap_id;
} sHOSTAP_DISABLED_NOTIFICATION;

typedef struct {
    int8_t vap_id;
} sHOSTAP_ENABLED_NOTIFICATION;

#define SSID_MAX_SIZE beerocks::message::WIFI_SSID_MAX_LENGTH
// ASSOCIATION_MAX_LENGTH is defined to be the maximum of the binary frame
// but since it is represented as hex here. we need two hex characters per byte + a terminating \0.
#define ASSOCIATION_FRAME_SIZE (2 * beerocks::message::ASSOCIATION_MAX_LENGTH + 1)
#define MAC_ADDR_SIZE 18
#define MAX_SUPPORTED_20M_CHANNELS beerocks::message::SUPPORTED_CHANNELS_LENGTH
#define MAX_SUPPORTED_CHANNELS                                                                     \
    (MAX_SUPPORTED_20M_CHANNELS * 4) //max 64 channels, each with BW 20/40/80/160
} // namespace bwl

#endif // _BWL_BASE_WLAN_HAL_TYPES_H_

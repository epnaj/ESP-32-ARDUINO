#pragma once
#include <esp_now.h>
#include <iterator>
#include <algorithm>
#include <array>
#include <SPIFFS.h>
#include "string_split.hpp"
#include "my_MAC.hpp"
#include "psram_custom_allocator.hpp"

class KnownPeers {
    public:
    static constexpr std::size_t maxPeerCounter{ESP_NOW_MAX_TOTAL_PEER_NUM};
    /*
    Adds peer at the end of peer list.  
    Returns true if peer is added, false otherwise.
    */
    static bool add(const ps::vector <uint8_t> &peerAddress);
    /*
    Removes peer from the end of peer list.
    Returns true if peer is removed, false otherwise.
    */
    static bool remove();
    /*
    Removes specific peer if exists.
    Returns true if peer is removed, false otherwise.
    */
    static bool remove(const ps::vector <uint8_t> &peerAddress);
    /*
    Removes all peers.
    */
    static void clear();
    /*
    Get number of memorised peers.
    */
    static std::size_t number();
    /*
    Returns true if you can't add more peers.  
    False if it is possible to add another peer.
    */
    static bool full();
    /*
    @attention
    Might return empty vector in case index is not valid!
    */
    static ps::vector <uint8_t> get(const std::size_t &index);
    static std::array <std::array <uint8_t, ESP_NOW_ETH_ALEN>, maxPeerCounter>::iterator begin();
    static std::array <std::array <uint8_t, ESP_NOW_ETH_ALEN>, maxPeerCounter>::iterator end();
    static std::array <std::array <uint8_t, ESP_NOW_ETH_ALEN>, maxPeerCounter>::const_reverse_iterator rbegin();
    static std::array <std::array <uint8_t, ESP_NOW_ETH_ALEN>, maxPeerCounter>::const_reverse_iterator rend();
    // rarely seen order of private and public - just to make maxPeerCounter variable possible as public
    private:
    static std::size_t peerCounter;
    static std::array <std::array <uint8_t, ESP_NOW_ETH_ALEN>, KnownPeers::maxPeerCounter> peersAdded;
};

class EspNowPeerHandler {
    private:
    static constexpr auto peerFileExtension{".peer"};
    static constexpr auto peerDirectory{"/peers"};
    static ps::vector <uint8_t> stringToKey(const ps::string &keyString);
    static void checkFileSystem();
    /*  
    .peer file structure is as follows:  
    +------------+
    |mac address | pure BYTE form  
    |channel     | single INTEGER form  
    |interface   | single INTEGER form  
    |encrypt     | single BOOL form  
    |lmk         | pure BYTE form  
    +------------+  
    where lmk is written only if encrypt is TRUE
    */
    static void savePeerInFlash(const esp_now_peer_info_t &peerInfo);
    static void loadSinglePeerFromFlash(const String &path);
    static bool removeSinglePeerFromFlash(const ps::vector <uint8_t> &macVectorised);
    public:
    static void loadPeersFromFlash();
    static void addPeer(
        const ps::string &mac, 
        const ps::string &channel, 
        const ps::string &interface, 
        const ps::string &lmk
    );
    static void removePeer(const ps::string &mac);
    static void setPMK(const ps::string &keyString);
    static void printPeer(const esp_now_peer_info_t &peerInfo);
};
#include "esp_now_peer_handler.hpp"

// static members init
std::size_t KnownPeers::peerCounter(0);
std::array <std::array <uint8_t, ESP_NOW_ETH_ALEN>, KnownPeers::maxPeerCounter> KnownPeers::peersAdded;
// ps::vector <ps::vector <uint8_t>> KnownPeers::peersAdded;
// ps::vector <ps::vector <uint8_t>> KnownPeers::peersAdded(KnownPeers::maxPeerCounter, {0, 0, 0, 0, 0, 0});

std::array <std::array <uint8_t, ESP_NOW_ETH_ALEN>, KnownPeers::maxPeerCounter>::iterator KnownPeers::begin() {
    return std::begin(KnownPeers::peersAdded);
}
std::array <std::array <uint8_t, ESP_NOW_ETH_ALEN>, KnownPeers::maxPeerCounter>::iterator KnownPeers::end() {
    return std::next(std::begin(KnownPeers::peersAdded), KnownPeers::peerCounter);
}
std::array <std::array <uint8_t, ESP_NOW_ETH_ALEN>, KnownPeers::maxPeerCounter>::const_reverse_iterator KnownPeers::rbegin() {
    return std::next(KnownPeers::peersAdded.rend(), KnownPeers::peerCounter);
}
std::array <std::array <uint8_t, ESP_NOW_ETH_ALEN>, KnownPeers::maxPeerCounter>::const_reverse_iterator KnownPeers::rend() {
    return KnownPeers::peersAdded.rend();
}

bool KnownPeers::add(const ps::vector <uint8_t> &peerAddress) {
    if(KnownPeers::peerCounter < KnownPeers::maxPeerCounter){
        std::copy(
            std::begin(peerAddress),
            std::end(peerAddress),
            std::begin(KnownPeers::peersAdded[KnownPeers::peerCounter++])
        );
        // ++KnownPeers::peerCounter;
        return true;
    }
    return false;
}

bool KnownPeers::remove(const ps::vector <uint8_t> &peerAddress) {
    for(auto &peerAddr : KnownPeers()) {
        if(
            peerAddr.size() == peerAddress.size() && 
            std::equal(std::begin(peerAddr), std::end(peerAddr), std::begin(peerAddress))
        ) {
            std::swap(peerAddr, KnownPeers::peersAdded[peerCounter]);
            --peerCounter;
            return true;
        }
    }
    return false;
}

bool KnownPeers::remove() {
    if(peerCounter){
        --peerCounter;
        return true;
    }
    return false;
}

void KnownPeers::clear() {
    peerCounter = 0;
}

std::size_t KnownPeers::number() {
    return peerCounter;
}

bool KnownPeers::full(){
    return KnownPeers::peerCounter == KnownPeers::maxPeerCounter;
}

ps::vector <uint8_t> KnownPeers::get(const std::size_t &index){
    // the copy is in fact inevitable, so why don't I interface it through ps::vector?
    return (0 <= index && index < KnownPeers::peerCounter) ? 
        ps::vector <uint8_t> (std::begin(KnownPeers::peersAdded[index]), std::end(KnownPeers::peersAdded[index])) : 
        ps::vector <uint8_t> {};
}

ps::vector <uint8_t> EspNowPeerHandler::stringToKey(const ps::string &keyString){
    ps::string hexString = filterNonHex(ps::string(keyString.c_str()));
    ps::vector <uint8_t> output(16, 0);
    
    for (unsigned int i = 0, counter = 0; i < hexString.size() && counter < output.size(); i += 2, ++counter) {
        ps::string byteString = hexString.substr(i, 2);
        uint8_t byte = static_cast <uint8_t> (strtol(byteString.c_str(), nullptr, 16));
        output[counter] = byte;
    }
    return output;
}

void EspNowPeerHandler::checkFileSystem(){
    if(!SPIFFS.begin())
        return;
    if(!SPIFFS.exists(EspNowPeerHandler::peerDirectory)){
        SPIFFS.mkdir(EspNowPeerHandler::peerDirectory);
    }
}

void EspNowPeerHandler::savePeerInFlash(const esp_now_peer_info_t &peerInfo) {
    EspNowPeerHandler::checkFileSystem();
    // there shouldn't be more than 20 peer files as this is the value corresponding to ESP_NOW_MAX_TOTAL_PEER_NUM.
    if(KnownPeers::full() || !KnownPeers::number()){
        return;
    }
    // for(std::size_t peerNumberCounter{1}; peerNumberCounter <= KnownPeers::maxPeerCounter; ++peerNumberCounter){
    //     auto currentPeerFile = String(EspNowPeerHandler::peerDirectory) + "/" + String(peerNumberCounter) + String(EspNowPeerHandler::peerFileExtension);
    //     if(SPIFFS.exists(currentPeerFile)) {
    //         EspNowPeerHandler::loadSinglePeerFromFlash(currentPeerFile);
    //     }
    // }
    fs::File file = SPIFFS.open(
        String(KnownPeers::number()) + String(EspNowPeerHandler::peerFileExtension),
        FILE_WRITE,
        true
    );
    if(file.availableForWrite()){
        file.write(peerInfo.peer_addr, ESP_NOW_ETH_ALEN);
        file.write('\n');
        file.write(peerInfo.channel);
        file.write('\n');
        file.write(peerInfo.ifidx);
        file.write('\n');
        file.write(peerInfo.encrypt);
        file.write('\n');
        if(peerInfo.encrypt){
            file.write(peerInfo.lmk, ESP_NOW_KEY_LEN);
            file.write('\n');
        }
    }
    file.close();
}

void EspNowPeerHandler::loadSinglePeerFromFlash(const String &path){
    EspNowPeerHandler::checkFileSystem();
    if(KnownPeers::full()){
        return;
    }
    fs::File file = SPIFFS.open(
        path,
        FILE_READ
    );
    esp_now_peer_info_t peerInfo;
    if(file.available()){
        file.readBytesUntil('\n', peerInfo.peer_addr, ESP_NOW_ETH_ALEN);
        file.readBytesUntil('\n', &peerInfo.channel, std::size_t{1});

        uint8_t tmpWifi[sizeof(wifi_interface_t)];
        file.readBytesUntil('\n', tmpWifi, sizeof(wifi_interface_t));
        peerInfo.ifidx = static_cast <wifi_interface_t> (*tmpWifi);

        uint8_t tmpEncrypt[sizeof(bool)];
        file.readBytesUntil('\n', tmpEncrypt, sizeof(bool));
        peerInfo.encrypt = static_cast <bool> (*tmpEncrypt);

        if(peerInfo.encrypt){
            file.readBytesUntil('\n', peerInfo.lmk, ESP_NOW_KEY_LEN);
        }
    }
    auto exists = esp_now_is_peer_exist(peerInfo.peer_addr);
    if(!exists){
        auto error = esp_now_add_peer(&peerInfo);
        if(error == ESP_ERR_ESPNOW_NOT_INIT){
            error = esp_now_init();
            ESP_ERROR_CHECK_WITHOUT_ABORT(error);
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_now_add_peer(&peerInfo));
        }
        ps::vector <uint8_t> macVectorised(6, 0);
        std::copy(peerInfo.peer_addr, peerInfo.peer_addr + ESP_NOW_ETH_ALEN, std::begin(macVectorised));
        KnownPeers::add(macVectorised);
    }
    file.close();
}

void EspNowPeerHandler::loadPeersFromFlash() {
    EspNowPeerHandler::checkFileSystem();
    for(std::size_t peerNumberCounter{1}; peerNumberCounter <= KnownPeers::maxPeerCounter; ++peerNumberCounter){
        auto currentPeerFile = String(EspNowPeerHandler::peerDirectory) + "/" + String(peerNumberCounter) + String(EspNowPeerHandler::peerFileExtension);
        if(SPIFFS.exists(currentPeerFile)) {
            EspNowPeerHandler::loadSinglePeerFromFlash(currentPeerFile);
        }
    }
}

bool EspNowPeerHandler::removeSinglePeerFromFlash(const ps::vector <uint8_t> &macVectorised){
    EspNowPeerHandler::checkFileSystem();
    for(std::size_t peerNumberCounter{1}; peerNumberCounter <= KnownPeers::maxPeerCounter; ++peerNumberCounter){
        auto currentPeerFile = String(EspNowPeerHandler::peerDirectory) + "/" + String(peerNumberCounter) + String(EspNowPeerHandler::peerFileExtension);
        if(SPIFFS.exists(currentPeerFile)) {
            fs::File peerFile = SPIFFS.open(currentPeerFile);
            uint8_t tmp[ESP_NOW_ETH_ALEN];
            peerFile.readBytesUntil('\n', tmp, ESP_NOW_ETH_ALEN);
            if(std::equal(tmp, tmp + ESP_NOW_ETH_ALEN, std::begin(macVectorised))) {
                SPIFFS.remove(currentPeerFile);
                peerFile.close();
                return true;
            }
            peerFile.close();
        }
    }
    return false;
}


void EspNowPeerHandler::addPeer(
    const ps::string &mac, 
    const ps::string &channel, 
    const ps::string &interface, 
    const ps::string &lmk
) {
    if(mac == "" || channel == "" || interface == "") {
        std::cout << "Missing mendatory arguments!\n";
        return;
    }
    uint8_t channelInt            = static_cast <uint8_t> (std::atol(channel.c_str()));
    wifi_interface_t interfaceInt = static_cast <wifi_interface_t> (std::atol(interface.c_str()));
    esp_now_peer_info_t peerInfo  = {
        .channel   = channelInt,
        .ifidx     = interfaceInt,
        .encrypt   = false,
    };

    ps::vector <uint8_t> macVectorised{MyMAC(mac).get()};
    if(!macVectorised.size()){
        std::cout << "PEER NOT ADDED!\n";
        return;
    }

    std::copy(macVectorised.begin(), macVectorised.end(), peerInfo.peer_addr);
    
    ps::vector <uint8_t> lmkVectorised = stringToKey(lmk);
    if(lmk.size()){
        peerInfo.encrypt = true;
        std::copy(lmkVectorised.begin(), lmkVectorised.end(), peerInfo.lmk);
    }

    auto err = esp_now_add_peer(&peerInfo);
    std::cout << "PEER " << (err ? "NOT " : "") << "ADDED!\n";
    if(!err) {
        KnownPeers::add(macVectorised);
        // EspNowPeerHandler::savePeerInFlash(peerInfo);
        std::cout << "MAC       "; MyMAC::printMac(macVectorised); std::cout << std::endl;
        std::cout << "CHANNEL   " << static_cast <int> (peerInfo.channel) << std::endl;
        std::cout << "INTERFACE " << static_cast <int> (peerInfo.ifidx)   << std::endl;
        std::cout << "ENCRYPT   " << std::boolalpha << peerInfo.encrypt   << std::noboolalpha << std::endl;
        if(peerInfo.encrypt) {
            std::cout << "LMK       "; 
            std::copy(lmkVectorised.begin(), std::prev(lmkVectorised.end()), std::ostream_iterator <int> (std::cout << std::hex, " "));
            std::cout << std::endl;
        }
    }
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
}

void EspNowPeerHandler::removePeer(const ps::string &mac){
    if(mac == "") {
        std::cout << "Missing mendatory arguments!\n";
        return;
    }
    ps::vector <uint8_t> macVectorised{MyMAC(mac).get()};
    if(KnownPeers::remove(macVectorised)){
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_now_del_peer(macVectorised.data()));
        // EspNowPeerHandler::removeSinglePeerFromFlash(macVectorised);
        std::cout << "PEER: "; MyMAC::printMac(macVectorised); std::cout << " DELETED!\n";
        return;
    }
    std::cout << "PEER: "; MyMAC::printMac(macVectorised); std::cout << " NOT DELETED!\n";
}

void EspNowPeerHandler::setPMK(const ps::string &keyString) {
    ESP_ERROR_CHECK(esp_now_set_pmk(EspNowPeerHandler::stringToKey(keyString).data()));
}

void EspNowPeerHandler::printPeer(const esp_now_peer_info_t &peerInfo) {
    std::cout << "MAC       "; MyMAC::printMac(peerInfo.peer_addr, ESP_NOW_ETH_ALEN); std::cout << std::endl;
    std::cout << "CHANNEL   " << static_cast <int> (peerInfo.channel) << std::endl;
    std::cout << "INTERFACE " << static_cast <int> (peerInfo.ifidx)   << std::endl;
    std::cout << "ENCRYPT   " << std::boolalpha << peerInfo.encrypt   << std::noboolalpha << std::endl;
    if(peerInfo.encrypt) {
        std::cout << "LMK       "; 
        std::copy(peerInfo.lmk, peerInfo.lmk + ESP_NOW_KEY_LEN, std::ostream_iterator <int> (std::cout << std::hex, " "));
        std::cout << std::endl;
    } 
}
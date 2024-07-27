#include "commands.hpp"

bool Empty::handle(ps::map <ps::string, ps::string> &parsedArguments) {
    // std::cout << std::endl;
    return true;
}

ps::string Empty::printHelp() {
    return "";
}

String Passwd::salt         = "b5390w84kgh35ghk0980ajhsg2asf";
String Passwd::passwordHash = "c55915076201f792d94850f5ba9ecfcc26ebf0ad8bedec63d32a42d5bcb1d9ba";

bool Passwd::masterAuthenticate() {
    std::cout << "Enter MASTER (GLOBAL) password:\n";
    // salted hash of hardcoded passowrd
    // M@5terhi
    // ugly to keep it cached
    if(sha256((Passwd::salt + readSerial()).c_str()) != ps::string(Passwd::passwordHash.c_str())){
        std::cout << "Wrong password!\n";
        return false;
    }
    return true;
};

bool Passwd::handle(ps::map <ps::string, ps::string> &parsedArguments) {
    std::cout << "Enter new MASTER (GLOBAL) password:\n";
    auto passwordFirst = sha256((Passwd::salt + readSerial()).c_str());
    std::cout << "Enter new MASTER (GLOBAL) password once again:\n";
    auto passwordSecond = sha256((Passwd::salt + readSerial()).c_str());
    if(passwordFirst == passwordSecond) {
        Passwd::passwordHash = String(passwordFirst.c_str());
        std::cout << "MASTER password changed!\n";
        return true;
    }
    std::cout << "Passwords don't match! MASTER Passsword is not changed.\n";
    return true;
}

ps::string Passwd::printHelp() {
    return "passwd *NO FLAGS*";
}

bool PeerAdd::handle(ps::map <ps::string, ps::string> &parsedArguments) {
    EspNowPeerHandler::addPeer(
        parsedArguments["-m"],
        parsedArguments["-c"],
        parsedArguments["-i"],
        parsedArguments["-l"]
    );
    return true;
}

ps::string PeerAdd::printHelp() {
    return "peeradd -m <MAC> -c <CHANNEL> -i <INTERFACE> -l [LOCAL MASTER KEY]";
}

bool PeerRemove::handle(ps::map <ps::string, ps::string> &parsedArguments) {
    EspNowPeerHandler::removePeer(
        parsedArguments["-m"]
    );
    return true;
}

ps::string PeerRemove::printHelp() {
    return "peerremove -m <MAC>";
}

bool PeerList::handle(ps::map <ps::string, ps::string> &parsedArguments) {
    bool detailed = parsedArguments.find("-d") != std::end(parsedArguments);
    if(!KnownPeers::number()) {
        std::cout << "LIST IS EMPTY!" << std::endl; // tu flush
        return true;
    }
    esp_now_peer_info_t peerInfo;
    for(const auto &registeredMac : KnownPeers()) {
        if(detailed){
            esp_now_get_peer(registeredMac.data(), &peerInfo);
            EspNowPeerHandler::printPeer(peerInfo);
        } else {
            MyMAC::printMac(registeredMac); std::cout << std::endl;
        }
    }
    return true;
}

ps::string PeerList::printHelp() {
    return "peerlist [-d (print with details)]";
}

bool Exit::handle(ps::map <ps::string, ps::string> &parsedArguments) {
    // here "logout"
    Serial.end();
    // here break the loop
    Serial.begin(9600);
    // constant 9600, doesn't matter at the end of the day
    return false;
}

ps::string Exit::printHelp() {
     return "passwd *NO FLAGS*";
}

bool Help::handle(ps::map <ps::string, ps::string> &parsedArguments) {
//     std::cout << R"""(Current commands:
// passwd          - password change
// peeradd         - add esp-now peer
// peerremove      - remove esp-now peer
// peerlist        - list registered esp-now peers
// pmk             - set Private Master Key for ESP-NOW
// sleep           - enter deep sleep instantly
// exit            - exit
// help            - well :)
// )""";
    std::cout << Help::printHelp() << std::endl;
    return true;
}

ps::string Help::printHelp() {
    // ps::map <ps::string, ps::string> m{};
    // handle(m);
    return R"""(Current commands:
passwd          - password change
peeradd         - add esp-now peer
peerremove      - remove esp-now peer
peerlist        - list registered esp-now peers
pmk             - set Private Master Key for ESP-NOW
sleep           - enter deep sleep instantly
exit            - exit
help            - well :))""";
}

bool PMK::handle(ps::map <ps::string, ps::string> &parsedArguments) {
    if(parsedArguments["-k"] != ""){
        EspNowPeerHandler::setPMK(parsedArguments["-k"]);
        std::cout << "PMK SET!\n";
    }
    std::cout << "Missing mandatory arguments!" << std::endl;
    return true;
}

ps::string PMK::printHelp() {
    return "pmk -k <PRIVATE MASTER KEY>";
}

bool Sleep::handle(ps::map <ps::string, ps::string> &parsedArguments){
    esp_deep_sleep_start();
}

ps::string Sleep::printHelp() {
    return "sleep *NO FLAGS*";
}
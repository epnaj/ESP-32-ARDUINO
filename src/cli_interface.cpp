#include "cli_interface.hpp"

constexpr auto promptString = ">";

void prompt() {
    // well here Serial Adruino wins :(
    Serial.write(promptString);
}

CLI::CLI() {
    commandMap.emplace(std::make_pair("",           std::unique_ptr <Command> (new Empty)));
    commandMap.emplace(std::make_pair("passwd",     std::unique_ptr <Command> (new Passwd)));
    commandMap.emplace(std::make_pair("peeradd",    std::unique_ptr <Command> (new PeerAdd)));
    commandMap.emplace(std::make_pair("peerremove", std::unique_ptr <Command> (new PeerRemove)));
    commandMap.emplace(std::make_pair("peerlist",   std::unique_ptr <Command> (new PeerList)));
    commandMap.emplace(std::make_pair("pmk",        std::unique_ptr <Command> (new PMK)));
    commandMap.emplace(std::make_pair("exit",       std::unique_ptr <Command> (new Exit)));
    commandMap.emplace(std::make_pair("help",       std::unique_ptr <Command> (new Help)));
    commandMap.emplace(std::make_pair("sleep",      std::unique_ptr <Command> (new Sleep)));
}

ps::map <ps::string, ps::string> CLI::parseListToMap(const ps::list <ps::string> &args) {
    ps::map <ps::string, ps::string> argumentsMap;
    std::pair <ps::string, ps::string> tmpPair;
    for(const auto &segment : args){
        if(segment[0] == '-'){
            if(!tmpPair.first.empty()){
                argumentsMap.insert(tmpPair);
                tmpPair = std::pair <ps::string, ps::string> ();
            }
            tmpPair.first = segment;
        } else if(!tmpPair.first.empty()) {
            tmpPair.second = segment;
            argumentsMap.insert(tmpPair);
            tmpPair = std::pair <ps::string, ps::string> ();
        }
    }
    if(!tmpPair.first.empty()){
        argumentsMap.insert(tmpPair);
    }
    return argumentsMap;
}

bool CLI::menuEntry(){
    ps::list<ps::string> command                  = split(ps::string(readSerial().c_str()), " ");
    ps::map <ps::string, ps::string> argumentsMap = parseListToMap(command);

    if(
        command.size() > 1 && commandMap.count(command.front()) &&
        (argumentsMap.find("-h") != argumentsMap.end() || argumentsMap.find("--help") != argumentsMap.end())
    ) {
        std::cout << commandMap[command.front()]->printHelp() << std::endl;
        return true;
    }
    
    if(commandMap.count(command.front())){
        return commandMap[command.front()]->handle(argumentsMap);
    }
    // std::cout << "U can always call \"help\"!\n";
    return true;
}

void CLI::beginAsync(){
    this->cliThread = std::thread(
        [&]() -> void {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            while(true){
                while(!Passwd::masterAuthenticate()){
                    // prompt();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
                std::cout << "Commmands: \n";
                for(const auto &p : commandMap) {
                    // just for now we filter empty
                    if(p.first != "") {
                        std::cout << p.first << std::endl;
                    }
                }
                prompt();
                while(menuEntry()){
                    prompt();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            }
        }
    );
}

CLI cli;
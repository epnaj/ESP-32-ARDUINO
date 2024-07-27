// arduino utilities
#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>

// cpp std
#include <iostream>
#include <vector>
#include <array>
#include <thread>
#include <chrono>

// espressif utilities
#include <esp_now.h>
#include <nvs_flash.h>
#include <esp_system.h>

// my implementations
#include <my_MAC.hpp>
#include <my_leds.hpp>
#include <value_mutex.hpp>
#include <touch_pin_reader_struct.hpp>
#include <my_esp_now_vector_converter.hpp>
#include <cli_interface.hpp>
#include <psram_custom_allocator.hpp>
#include <esp_now_peer_handler.hpp>


constexpr gpio_num_t BUILTIN_LED_GPIO = GPIO_NUM_21;
constexpr uint8_t WIFI_CHANNEL        = 0;
constexpr auto MY_WIFI_IF             = WIFI_IF_STA;
constexpr auto MY_WIFI_MODE           = WIFI_MODE_STA;
constexpr auto A_COM_6                = 1;
constexpr uint8_t localMasterKey[]    = "abcdefghijklmnop";

template <class Allocator = std::allocator <uint8_t>>
void send_using_esp_now(const std::vector <uint8_t, Allocator> &data, const std::vector <uint8_t, Allocator> &macAddress) {
    ESP_ERROR_CHECK(esp_now_send(macAddress.data(), data.data(), data.size()));
}

template <class Allocator = std::allocator <uint8_t>>
void send_using_esp_now(const std::vector <uint8_t, Allocator> &data, const std::array <uint8_t, ESP_NOW_ETH_ALEN> &macAddress) {
    ESP_ERROR_CHECK(esp_now_send(macAddress.data(), data.data(), data.size()));
}

template <class Allocator = std::allocator <uint8_t>>
void send_using_esp_now(const std::vector <uint8_t, Allocator> &data, MyMAC &macAddress) {
    ESP_ERROR_CHECK(esp_now_send(macAddress.get().data(), data.data(), data.size()));
}

template <class Allocator = std::allocator <uint8_t>>
void send_using_esp_now(const std::vector <uint8_t, Allocator> &data) {
    // CANT BE NULLPTR
    ESP_ERROR_CHECK(esp_now_send(NULL, data.data(), data.size()));
}

void tempHandler(const uint8_t *data, int data_len){
    // std::cout << "I'm in tempHandler!\n";
    std::vector <uint8_t> v_temp(data_len);
    std::copy(data, data + data_len, v_temp.begin());
    auto blinkInformationVector = EspNowVectorConverter::toEspConvertableClass <BlinkInformation> (v_temp);
    for(auto &blinkInfo : blinkInformationVector) {
        // std::cout << "I successfully converted BlinkInformaton to: " << blinkInfo.blink_length << " " << blinkInfo.delay_between_blinks << " " << blinkInfo.n << std::endl;
        MyLEDGPIO(BUILTIN_LED_GPIO).blink_led_n_times(
            blinkInfo.blink_length, 
            blinkInfo.delay_between_blinks, 
            blinkInfo.n
        );
    }
}

void receive_using_esp_now(const uint8_t *mac_addr, const uint8_t *data, int data_len){
    printf("MAC ADDRESS RECEIVED: " MACSTR "\n", MAC2STR(mac_addr));
    // don't care about coincidential data for now
    if(data_len > 1 && data[0] == 21 && data[1] == 37){
        MyLEDGPIO(BUILTIN_LED_GPIO).blink_led_n_times(100, 100, 10);
        std::cout << "DO SPIULKOLOTU!\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        esp_deep_sleep_start();
    }

    if(data_len > 4){
        std::thread(tempHandler, data, data_len).detach();
        return;
    }
}

void send_esp_now_callback(const uint8_t *mac_address, esp_now_send_status_t status) {
    if(status == ESP_NOW_SEND_SUCCESS){
        std::cout<< "SEND SUCCESS\n";
    } else {
        std::cout<< "SEND FAILURE\n";
    }
}

// passing by copy as it will be deleted resource anyway
void setWakeOnTouch(TouchPinReaderVector touchPinReaderVector) {
    for(auto &tpr : touchPinReaderVector.v){
        touchSleepWakeUpEnable(tpr.tp.getPinNum(), tpr.tp.getTreshold());
    }
}

void setLightOnTouch(TouchPinReaderVector &touchPinReaderVector){
    for(size_t i=0; i<touchPinReaderVector.v.size(); ++i){
        std::cout << "REGISTERING PIN: " << static_cast <int> (touchPinReaderVector.v[i].tp.getPinNum()) << std::endl;
        auto wakeUpCallback = [](void *pin_mutex) -> void {
            auto tp = static_cast <TouchPin *> (pin_mutex);
            if(!tp->isON()){
                tp->set(true);
            }
        };
        touchAttachInterruptArg(touchPinReaderVector.v[i].tp.getPinNum(), wakeUpCallback, &touchPinReaderVector.v[i].tp, touchPinReaderVector.v[i].tp.getTreshold()); 
    }
}

void keepPinYelling(TouchPin &touchPin){
    auto builtInLed = MyLEDGPIO(BUILTIN_LED_GPIO);
    while(touchRead(touchPin.getPinNum()) >= touchPin.getTreshold()){
        builtInLed.blink_led_n_times(10, 0, 1);
    }
    // for now it's completley deadlock/collision safe :)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    touchPin.set(false);
}

void yellLitPins(TouchPinReaderVector &touchPinReaderVector){
    for (uint8_t i = 0; i < touchPinReaderVector.v.size(); ++i){
        if(touchPinReaderVector.v[i].tp.isON()){
            std::cout << "Pin " << static_cast <int> (touchPinReaderVector.v[i].tp.getPinNum()) << " touched!\n";
            touchPinReaderVector.v[i].pinTaskThread = std::thread(
                keepPinYelling,
                std::ref(touchPinReaderVector.v[i].tp)
            );
            touchPinReaderVector.v[i].pinTaskThread.join();
        }
    }
}

void readGlobalQueue(TouchPinReaderVector &touchPinReaderVector){
    std::cout << "Queue Thread started\n";
    // std::condition_variable myCondition;
    // myCondition.notify_one();
    while(true){
        if(TouchPin::changed.get()){
            yellLitPins(touchPinReaderVector);
            TouchPin::changed.set(false);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


uint32_t randomRange(uint32_t min_val, uint32_t max_val) {
    return min_val + (esp_random() % (max_val - min_val));
}

void sendRandomBlinkEspNOW(){
    // uint32_t blink_length, uint32_t delay_between_blinks, unsigned int n
    send_using_esp_now(
        EspNowVectorConverter::classToByteArray <BlinkInformation> ({
            {
                randomRange(100, 250), 
                100,
                randomRange(1, 5),
            }
        })
    );
}

void displayFileForClient(ps::string path, WiFiClient &client){
    fs::File file = SPIFFS.open(path.c_str(), FILE_READ);
    if(!file){
        std::cout << "File: " << path << " is missing!\n";
    }

    ps::string buffer(file.size(), ' ');
    int i{0};
    while(file.available()){
        buffer[i++] = file.read();
    }
    client.println(buffer.c_str());
}

TouchPinReaderVector touchPinReaderVector;
void setup() {
    Serial.begin(9600);
    delay(1000);

    std::cout << "BEGIN OF PROGRAM\n";
    // ESP_ERROR_CHECK(nvs_flash_erase());
    // ESP_ERROR_CHECK(nvs_flash_init());
    SPIFFS.begin(true);
    // inital phase should be blocking
    // initialPhase();
    WiFi.mode(MY_WIFI_MODE);
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(send_esp_now_callback));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(receive_using_esp_now));
    // EspNowPeerHandler::loadPeersFromFlash();
    MyLEDGPIO(BUILTIN_LED_GPIO).single_blink(250);
    /*
    MyMAC macA("DC:DA:0C:57:4C:9C");
    MyMAC macB("DC:DA:0C:57:56:48");
    */
    esp_sleep_enable_touchpad_wakeup();
    setWakeOnTouch(TouchPinReaderVector({TOUCH_PAD_NUM7}, 20000));
    touchPinReaderVector = TouchPinReaderVector({TOUCH_PAD_NUM1, TOUCH_PAD_NUM6}, 20000);
    setLightOnTouch(touchPinReaderVector);
    std::thread(readGlobalQueue, std::ref(touchPinReaderVector)).detach();
    MyLEDGPIO(BUILTIN_LED_GPIO).blink_led_n_times(100, 100, 5);
    cli.beginAsync();
    // std::thread(
    //     [&](){
    //         while(true){
    //             if(KnownPeers::number()){
    //                 for(const auto &mac : KnownPeers()) {
    //                     for(auto i=0; i<10; ++i){
    //                         sendRandomBlinkEspNOW();
    //                         MyLEDGPIO(BUILTIN_LED_GPIO).blink_led_n_times(500, 500, 2);
    //                     }
    //                     std::cout << "SENDING SLEEP CODE TO: "; MyMAC::printMac(mac); std::cout << std::endl;
    //                     send_using_esp_now({21, 37}, mac); // sleep code
    //                 }
    //                 std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //                 // esp_deep_sleep_start();
    //             }
    //             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //         }
    //     }
    // ).detach();
}

void loop() {}
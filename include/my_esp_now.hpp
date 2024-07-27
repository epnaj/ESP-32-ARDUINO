#pragma once
#include <Arduino.h>
#include <iostream>
#include <vector>
#include <functional>

#include <esp_now.h>
#include "esp_now_peer_handler.hpp"
#include "psram_custom_allocator.hpp"

class MyESPNOW {
    private:

    public:
    MyESPNOW();
    // ESP_ERROR_CHECK(esp_now_init());
    void addSendCallback(esp_now_send_cb_t sendCallback);
        // ESP_ERROR_CHECK(esp_now_register_send_cb(sendCallback));
    // can't convert to std::function
    void addReceiveCallback(esp_now_recv_cb_t receiveCallback);
        // ESP_ERROR_CHECK(esp_now_register_recv_cb(receiveCallback));
    void deleteSendCallback();
        // ESP_ERROR_CHECK(esp_now_unregister_send_cb());
    void deleteReceiveCallback();
        // ESP_ERROR_CHECK(esp_now_unregister_recv_cb());
    void addPeer();
    void removePeer();

    template <typename T, class Allocator>
    void sendData();

    template <typename T, class Allocator>
    void sendData();
};
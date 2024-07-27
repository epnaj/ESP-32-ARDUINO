#pragma once
#include <string>
#include <Arduino.h>
#include "psram_custom_allocator.hpp"

/*
Read from Serial until '\\n' character.
BLOCKING
*/
String readSerial(bool blocking = true);

/*
Write to Serial with '\\n' character.
*/
void writeSerial(const String &data);

/*
Write to Serial with '\\n' character.
*/
void writeSerial(const ps::string &data);

/*
Write to Serial with '\\n' character.
*/
void writeSerial(const char *data);
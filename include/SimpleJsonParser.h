#ifndef SIMPLEJSONPARSER_h
#define SIMPLEJSONPARSER_h

#include <Arduino.h>
#include <string>

#ifdef ESP32
#include <FS.h>
#include <SPIFFS.h>
#elif defined(ESP8266)
#endif

#include <SPIFFSEditor.h>


class SimpleJsonParser
{
private:
    /* data */
public:
    SimpleJsonParser();
    ~SimpleJsonParser();
    String getJSONValueQuickByKey(String path, String key){return getJSONValueByKey(fileToString(path),key);}; 
    String fileToString(String path);
    String getJSONValueByKey(String text, String key);
};

#endif
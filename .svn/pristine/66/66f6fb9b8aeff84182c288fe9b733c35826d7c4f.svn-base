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
    void SkipWhiteSpace(File f);
    
public:
    SimpleJsonParser();
    ~SimpleJsonParser();
    String getJSONValueByKeyFromFile(String path, String key);
    String getJSONValueByKey(String path, String key){return getJSONValueByKeyFromString(fileToString(path),key);}; 
    String fileToString(String path);
    String getJSONValueByKeyFromString(String text, String key);
};

#endif
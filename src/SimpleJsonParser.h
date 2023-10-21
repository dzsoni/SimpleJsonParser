#ifndef SIMPLEJSONPARSER_h
#define SIMPLEJSONPARSER_h

#include <Arduino.h>
#include <WString.h>

#ifdef ESP32
#include <FS.h>
#include <SPIFFS.h>
#elif defined(ESP8266)
#endif

#include <SPIFFSEditor.h>

//#define DEBUG_SIMPLEJSON

#ifdef DEBUG_SIMPLEJSON
#define _SIMPLEJSON_PP(a) Serial.print(a);
#define _SIMPLEJSON_PL(a) Serial.println(a);
#else
#define _SIMPLEJSON_PP(a)
#define _SIMPLEJSON_PL(a)
#endif


class SimpleJsonParser
{
private:
    void _skipWhiteSpace(File f);
    int  _skipWhiteSpace(String jsontext, unsigned int frompos);
    
public:
    SimpleJsonParser();
    ~SimpleJsonParser();
    String getJSONValueByKeyFromFile(String path, String key);
    String fileToString(String path);
    String getJSONValueByKeyFromString(String text, String key);
    int    getNumberOfEntriesFromFile(String path);
    int    getNumberOfEntriesFromString(String jsontxt);
    String getJSONKeybyIndexFromFile(String path, uint32_t index);
    String getJSONKeybyIndexFromString(String jsontxt, int index);
    String getJSONValuebyIndexFromFile(String path,uint32_t index);
    String getJSONValuebyIndexFromString(String jsontxt,int index);
};

#endif
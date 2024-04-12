#ifndef SIMPLEJSONPARSER_h
#define SIMPLEJSONPARSER_h

#include <Arduino.h>
#include <WString.h>
#include <vector>
#include <utility>

#include "FS.h"

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
    String getValueByKeyFromFile(String path, String key);
    String fileToString(String path);
    String getValueByKeyFromString(String text, String key);
    int    getNumberOfEntriesFromFile(String path);
    int    getNumberOfEntriesFromString(String jsontxt);
    String getKeybyIndexFromFile(String path, uint32_t index);
    String getKeybyIndexFromString(String jsontxt, int index);
    String getValuebyIndexFromFile(String path,uint32_t index);
    String getValuebyIndexFromString(String jsontxt,int index);
    std::vector<std::pair<String,String>> extractKeysandValuesFromFile(String path);
};

#endif
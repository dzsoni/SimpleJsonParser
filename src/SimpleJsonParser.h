#ifndef SIMPLEJSONPARSER_h
#define SIMPLEJSONPARSER_h

#include <Arduino.h>
#include <WString.h>

#ifdef ESP32
#include <SPIFFS.h>
#elif defined(ESP8266)
#endif

#include <FS.h>

// #define DEBUG_SIMPLEJSON

#ifdef DEBUG_SIMPLEJSON
#define _SIMPLEJSON_PP(a) Serial.print(a);
#define _SIMPLEJSON_PL(a) Serial.println(a);
#else
#define _SIMPLEJSON_PP(a)
#define _SIMPLEJSON_PL(a)
#endif

template <typename T>
class SimpleJsonParser
{
private:
    T&   _obj;
    void _SkipWhiteSpace();

public:
    SimpleJsonParser(T &typ) : _obj(typ){};
    ~SimpleJsonParser(){};
    String GetValueByKey(String key);
    int GetNumberOfEntries();
    String getJSONKeybyIndex(int index);
    String getJSONValuebyIndex(int index);
};



template <typename T>
void SimpleJsonParser<T>::_SkipWhiteSpace()
{
    int s = _obj.peek();
    while (_obj.available() && (s == ' ' || s == '\n' || s == '\r' || s == '\t'))
    {
        _obj.read();
        s = _obj.peek();
    }
}

template <typename T>
String SimpleJsonParser<T>::GetValueByKey(String key)
{
    String value = "";
    _obj.seek(0,SeekSet);
    String searchPhrase = String("\"");
    searchPhrase.concat(key);
    searchPhrase.concat("\"");

    const char *charpt = searchPhrase.c_str();
    bool found = false;
    String s;
    char c;
    size_t relativepoz = 0;
    while (_obj.available())
    {
        relativepoz = 0;
        s = "";
        while (_obj.available())
        {

            if (((c = (char)_obj.read()) != charpt[relativepoz]) || (relativepoz + 1 > searchPhrase.length()) || (_obj.available() < (int)(searchPhrase.length() - relativepoz + 1)))
            {
                break;
            }
            relativepoz++;
            if (relativepoz == searchPhrase.length())
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            break;
        }
    }
    if (!found && !_obj.available())
    {
        _SIMPLEJSON_PL(F("Key not found."));
        return String("");
    }

    _SkipWhiteSpace();

    if ((c = (char)_obj.read()) != ':')
    {
        _SIMPLEJSON_PL(F("JSon error ':' is missing."));
        return String("");
    }
    _SkipWhiteSpace();

    if ((c = (char)_obj.read()) != '"')
    {
        _SIMPLEJSON_PL(F("JSon error '\"' is missing."));
        
        return String("");
    }

    while (((c = (char)_obj.read()) != '"') && _obj.available())
    {
        value += c;
    }
    if (!_obj.available())
    {
        _SIMPLEJSON_PL(F("JSon error. Shouldn't be end."));
        
        return String("");
    }
    
    return value;
}

template <typename T>
int SimpleJsonParser<T>::GetNumberOfEntries()
{
    _obj.seek(0,SeekSet);
    int count = 0;
    char c;
    while (_obj.available())
    {
        if ((c = (char)_obj.read()) == '"')
        {
            _SkipWhiteSpace();
            if ((c = (char)_obj.read()) == ':')
            {
                _SkipWhiteSpace();
                if ((c = (char)_obj.read()) == '"')
                {
                    count++;
                }
            }
        }
    }
    
    return count;
}
template <typename T>
String SimpleJsonParser<T>::getJSONKeybyIndex(int index)
{
    _obj.seek(0,SeekSet);
    uint32_t count = 0;
    uint32_t frstpt = 0, secpt = 0;
    String result;
    char c;
    while (_obj.available())
    {
        if ((((c = (char)_obj.read()) == ',' || (c == '{'))) && (!frstpt))
        {
            _SkipWhiteSpace();
            if ((c = (char)_obj.read()) == '"')
            {
                frstpt = _obj.position();
                continue;
            }
            else
            {
                _SIMPLEJSON_PL("Corrupt json:{ or , must follow a \"");
                
                return String("");
            }
        }

        if (c == '"')
        {
            secpt = _obj.position();
            if (secpt && frstpt && (_obj.available() >= 2))
            {
                _SkipWhiteSpace();
                if ((c = (char)_obj.read()) == ':')
                {
                    _SkipWhiteSpace();
                    if ((c = (char)_obj.read()) == '"')
                    {
                        if (index == count)
                        {
                            _obj.seek(frstpt, SeekSet);
                            while (frstpt < secpt - 1)
                            {
                                result += String((char)_obj.read());
                                frstpt++;
                            }
                            
                            return result;
                        }
                        count++;
                        frstpt = 0;
                        secpt = 0;
                    }
                }
            }
        }
    }
    
    return String("");
}
template <typename T>
String SimpleJsonParser<T>::getJSONValuebyIndex(int index)
{
    _obj.seek(0,SeekSet);
    uint32_t count = 0;
    uint32_t oldpt = 0, newpt = 0;
    String result;
    char c;

    while (_obj.available())
    {
        if ((c = (char)_obj.read()) == '"')
        {
            newpt = _obj.position();
            _SkipWhiteSpace();
            if ((c = (char)_obj.read()) == '}' && oldpt)
            {
                if (index == count)
                {
                    _obj.seek(oldpt, SeekSet);
                    while (oldpt < newpt - 1)
                    {
                        result += String((char)_obj.read());
                        oldpt++;
                    }
                    
                    return result;
                }
                count++; // shouldn't be here
            }
            else if (c == ':')
            {
                _SkipWhiteSpace();
                if ((c = (char)_obj.read()) == '"')
                {
                    oldpt = _obj.position();
                }
            }

            else if (c == ',' && oldpt)
            {
                _SkipWhiteSpace();
                if ((c = (char)_obj.read()) == '"')
                {
                    if (index == count)
                    {
                        _obj.seek(oldpt, SeekSet);
                        while (oldpt < newpt - 1)
                        {
                            result += String((char)_obj.read());
                            oldpt++;
                        }
                        
                        return result;
                    }
                    count++;
                    oldpt = 0;
                }
            }
        }
    }
    
    return String("");
}

#endif
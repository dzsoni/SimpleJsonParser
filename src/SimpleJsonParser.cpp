#include <SimpleJsonParser.h>

SimpleJsonParser::SimpleJsonParser()
{
}

SimpleJsonParser::~SimpleJsonParser()
{
}

String SimpleJsonParser::fileToString(String path)
{
    String content = "";

    // start spiff
    if (!SPIFFS.begin())
    {
        // Serious problem
        Serial.println(F("SPIFFS Mount failed"));

        return String("");
    } // end if

    // Serial.println(F("SPIFFS Mount succesfull."));

    // read file
    if (SPIFFS.exists(path))
    {
        File f = SPIFFS.open(path, "r");
        if (!f)
        {
            Serial.println(F("file open failed"));
            return String("");
        } // end if

        // read file

        while (f.available())
        {
            content += char(f.read());
        }
        f.close();
        return content; // f.readString();
    }
    return String("");
}

String SimpleJsonParser::getJSONValueByKeyFromString(String jsontext, String key)
{
    if (jsontext.length() == 0)
    {
        return String("");
    }

    String searchPhrase = String("\"");
    searchPhrase.concat(key);
    searchPhrase.concat("\"");
    int fromPosition = jsontext.indexOf(searchPhrase, 0);
    
    Serial.println(String("!")+String(fromPosition));
     if (fromPosition == -1)
    {
        // return because there is no status or it's null
        return String("");
    }
    fromPosition=fromPosition+key.length()+2;
    Serial.println(String("!!")+String(fromPosition)+String(jsontext.substring(fromPosition , fromPosition + 1)));

    fromPosition = _skipWhiteSpace(jsontext,fromPosition);
    if(jsontext.substring(fromPosition,fromPosition+1)!=":")
    {
        _SIMPLEJSON_PL(F("Missing ':' from JSON"));
        return String("");
    }
    fromPosition++;
    fromPosition = _skipWhiteSpace(jsontext,fromPosition);
    if(jsontext.substring(fromPosition,fromPosition+1)!="\"")
    {
        _SIMPLEJSON_PL(F("Missing '\"' from JSON"));
        return String("");
    }
    fromPosition++;
   
    int toPosition = jsontext.indexOf("\"", fromPosition);

    if (toPosition == -1)
    {
        // return because there is no end quote
        return String("");
    }
    jsontext.remove(toPosition);
    return jsontext.substring(fromPosition);
}

String SimpleJsonParser::getJSONValueByKeyFromFile(String path, String key)
{
    String value = "";

    // start spiff
    if (!SPIFFS.begin())
    {
        // Serious problem
        _SIMPLEJSON_PL(F("SPIFFS Mount failed."));

        return String("");
    } // end if

    // Serial.println(F("SPIFFS Mount succesfull."));

    // read file
    // Serial.println(path);
    if (SPIFFS.exists(path))
    {
        File f = SPIFFS.open(path, "r");
        if (!f)
        {
            _SIMPLEJSON_PL(F("File open failed."));
            return String("");
        }
        String searchPhrase = String("\"");
        searchPhrase.concat(key);
        searchPhrase.concat("\"");
        const char *charpt = searchPhrase.c_str();
        bool found = false;
        String s;
        char c;
        size_t relativepoz = 0;
        while (f.available())
        {
            relativepoz = 0;
            s = "";
            while (f.available())
            {

                if (((c = (char)f.read()) != charpt[relativepoz]) || (relativepoz + 1 > searchPhrase.length()) || (f.available() < (int)(searchPhrase.length() - relativepoz + 1)))
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
        if (!found && !f.available())
        {
            _SIMPLEJSON_PL(F("Key not found."));
            f.close();
            return String("");
        }

        _skipWhiteSpace(f);

        if ((c = (char)f.read()) != ':')
        {
            _SIMPLEJSON_PL(F("JSon file error ':' is missing."));
            f.close();
            return String("");
        }
        _skipWhiteSpace(f);

        if ((c = (char)f.read()) != '"')
        {
            _SIMPLEJSON_PL(F("JSon file error '\"' is missing."));
            f.close();
            return String("");
        }
        String value;
        while (((c = (char)f.read()) != '"') && f.available())
        {
            value += c;
        }
        if (!f.available())
        {
            _SIMPLEJSON_PL(F("JSon file error."));
            f.close();
            return String("");
        }
        f.close();
        return value;
    }
    return String("");
}
void SimpleJsonParser::_skipWhiteSpace(File f)
{
    if (f)
    {
        String s;
        while ((s = f.peek()) == " " || s == "\n" || s == "\r" || s == "\t")
        {
            f.read();
        }
    }
}

int SimpleJsonParser::_skipWhiteSpace(String jsontext, int frompos)
{
    String nextch;
    while (frompos < jsontext.length() )
    {
        nextch = jsontext.substring(frompos , frompos + 1);
        if (nextch == " " || nextch == "\n" || nextch == "\r" || nextch == "\t")
        {
            frompos++;
        }
        else{
            return frompos;
        }
    }
    return frompos;
}
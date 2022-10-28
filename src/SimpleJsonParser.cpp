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

    Serial.println(String("!") + String(fromPosition));
    if (fromPosition == -1)
    {
        // return because there is no status or it's null
        return String("");
    }
    fromPosition = fromPosition + key.length() + 2;
    Serial.println(String("!!") + String(fromPosition) + String(jsontext.substring(fromPosition, fromPosition + 1)));

    fromPosition = _skipWhiteSpace(jsontext, fromPosition);
    if (jsontext.substring(fromPosition, fromPosition + 1) != ":")
    {
        _SIMPLEJSON_PL(F("Missing ':' from JSON"));
        return String("");
    }
    fromPosition++;
    fromPosition = _skipWhiteSpace(jsontext, fromPosition);
    if (jsontext.substring(fromPosition, fromPosition + 1) != "\"")
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
    while (frompos < jsontext.length())
    {
        nextch = jsontext.substring(frompos, frompos + 1);
        if (nextch == " " || nextch == "\n" || nextch == "\r" || nextch == "\t")
        {
            frompos++;
        }
        else
        {
            return frompos;
        }
    }
    return frompos;
}
int SimpleJsonParser::getNumberOfObjectsFromFile(String path)
{
    int count = 0;
    char c;
    if (!SPIFFS.begin())
    {
        // Serious problem
        _SIMPLEJSON_PL(F("SPIFFS Mount failed."));

        return -1;
    } // end if

    if (SPIFFS.exists(path))
    {
        File f = SPIFFS.open(path, "r");
        if (!f)
        {
            _SIMPLEJSON_PL(F("File open failed."));
            return -1;
        }

        while (f.available())
        {
            if ((c = (char)f.read()) == '"')
            {
                _skipWhiteSpace(f);
                if (c = (char)f.read() == ':')
                {
                    _skipWhiteSpace(f);
                    if (c = (char)f.read() == '"')
                    {
                        count++;
                    }
                }
            }
        }
        f.close();
        return count;
    }
    return -1;
}

int SimpleJsonParser::getNumberOfObjectsFromString(String jsontxt)
{
    int count = 0;
    for (int i = 0; i < jsontxt.length(); i++)
    {
        if (jsontxt.substring(i, i + 1) == "\"")
        {
            i++;
            i = _skipWhiteSpace(jsontxt, i);
            if (jsontxt.substring(i, i + 1) == ":")
            {
                i++;
                i = _skipWhiteSpace(jsontxt, i);
                if (jsontxt.substring(i, i + 1) == "\"")
                {
                    count++;
                }
            }
        }
    }
    return count;
}

String SimpleJsonParser::getJSONKeybyIndexFromFile(String path, int index)
{
    uint32_t count = 0;
    uint32_t oldpt = 0, newpt = 0;
    String result;
    char c;
    if (!SPIFFS.begin())
    {
        // Serious problem
        _SIMPLEJSON_PL(F("SPIFFS Mount failed."));

        return String("");
    } // end if

    if (SPIFFS.exists(path))
    {
        File f = SPIFFS.open(path, "r");
        if (!f)
        {
            _SIMPLEJSON_PL(F("File open failed."));
            return String("");
        }

        while (f.available())
        {
            if ((c = (char)f.read()) == '"')
            {
                oldpt = newpt;
                newpt = f.position();
                _skipWhiteSpace(f);
                if (c = (char)f.read() == ':' && oldpt)
                {
                    _skipWhiteSpace(f);
                    if (c = (char)f.read() == '"')
                    {
                        if (index == count)
                        {
                            f.seek(oldpt, SeekSet);
                            while (oldpt < newpt - 1)
                            {
                                result += String((char)f.read());
                                oldpt++;
                            }
                            return result;
                        }
                        count++;
                        oldpt=0;
                        newpt=f.position();
                    }
                }
            }
        }
    }
    return String("");
}
String SimpleJsonParser::getJSONValuebyIndexFromFile(String path,int index)
{
    uint32_t count = 0;
    uint32_t oldpt = 0, newpt = 0;
    String result;
    char c;
    if (!SPIFFS.begin())
    {
        // Serious problem
        _SIMPLEJSON_PL(F("SPIFFS Mount failed."));

        return String("");
    } // end if

    if (SPIFFS.exists(path))
    {
        File f = SPIFFS.open(path, "r");
        if (!f)
        {
            _SIMPLEJSON_PL(F("File open failed."));
            return String("");
        }

        while (f.available())
        {
            if ((c = (char)f.read()) == '"')
            {
                oldpt = newpt;
                newpt = f.position();
                _skipWhiteSpace(f);
                c = (char)f.read();
                if (c  == '}' && oldpt)
                {
                    if (index == count)
                        {
                            f.seek(oldpt, SeekSet);
                            while (oldpt < newpt - 1)
                            {
                                result += String((char)f.read());
                                oldpt++;
                            }
                            return result;
                        }
                    count++;
                }
                else if (c == ',' && oldpt)
                {
                    _skipWhiteSpace(f);
                    if (c = (char)f.read() == '"')
                    {
                        if (index == count)
                        {
                            f.seek(oldpt, SeekSet);
                            while (oldpt < newpt - 1)
                            {
                                result += String((char)f.read());
                                oldpt++;
                            }
                            return result;
                        }
                        count++;
                        newpt=f.position();
                        oldpt=0;
                    }
                }
            }
        }
    }
    return String("");

}
String SimpleJsonParser::getJSONKeybyIndexFromString(String jsontxt, int index)
{
    int count = 0;
    uint32_t oldpt = 0, newpt = 0;
    for (uint32_t i = 0; i < jsontxt.length(); i++)
    {
        if (jsontxt.substring(i, i + 1) == "\"")
        {
            oldpt = newpt;
            newpt = i;
            i++;
            i = _skipWhiteSpace(jsontxt, i);
            if (jsontxt.substring(i, i + 1) == ":" && oldpt)
            {
                i++;
                i = _skipWhiteSpace(jsontxt, i);
                if (jsontxt.substring(i, i + 1) == "\"")
                {
                    if (index == count)
                    {
                        return jsontxt.substring(oldpt + 1, newpt);
                    }
                    count++;
                    oldpt=0;
                    newpt=i;
                }
            }
        }
    }
    return String("");
}

String SimpleJsonParser::getJSONValuebyIndexFromString(String jsontxt, int index)
{
    int count = 0;
    uint32_t oldpt = 0, newpt = 0;
    for (uint32_t i = 0; i < jsontxt.length(); i++)
    {
        if (jsontxt.substring(i, i + 1) == "\"")
        {
            oldpt = newpt;
            newpt = i;
            i++;
            i = _skipWhiteSpace(jsontxt, i);
            if (jsontxt.substring(i,i+1) == "}" && oldpt)
            {
                if (index == count)
                    {
                        return jsontxt.substring(oldpt + 1, newpt);
                    }
                count++;//shouldn't be here
            }
            else if (jsontxt.substring(i, i + 1) == "," && oldpt)
            {
                i++;
                i = _skipWhiteSpace(jsontxt, i);
                if (jsontxt.substring(i, i + 1) == "\"")
                {
                    if (index == count)
                    {
                        return jsontxt.substring(oldpt + 1, newpt);
                    }
                    count++;
                    oldpt=0;
                    newpt=i;
                }
            }
        }
    }
    return String("");
}
#include "SimpleJsonParser.h"

SimpleJsonParser::SimpleJsonParser()
{
}

SimpleJsonParser::~SimpleJsonParser()
{
}

/**
 * Retrieves the content of a file as a String.
 *
 * @param path The path to the file to read.
 *
 * @return The content of the file as a String.
 */
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
            _SIMPLEJSON_PL(String(F("File open failed.") + path));
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

/**
 * Retrieves the value associated with a given key from a JSON string.
 *
 * @param jsontext The JSON string to search.
 * @param key The key to search for.
 *
 * @return The value associated with the given key, or an empty string if the key is not found.
 */
String SimpleJsonParser::getValueByKeyFromString(String jsontext, String key)
{
    if (jsontext.length() == 0)
    {
        return String("");
    }

    String searchPhrase = String("\"");
    searchPhrase.concat(key);
    searchPhrase.concat("\"");
    int fromPosition = jsontext.indexOf(searchPhrase, 0);
    if (fromPosition == -1)
    {
        // return because there is no status or it's null
        return String("");
    }
    fromPosition = fromPosition + key.length() + 2;

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

/**
 * Retrieves the value associated with the specified key from a JSON file located at the given path.
 *
 * @param path The file path of the JSON file.
 * @param key The key to search for in the JSON file.
 *
 * @return The value corresponding to the provided key in the JSON file.
 *
 * @throws None
 */
String SimpleJsonParser::getValueByKeyFromFile(String path, String key)
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
            _SIMPLEJSON_PL(String(F("File open failed.") + path));
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
    else
    {
        _SIMPLEJSON_PL(String(F("File not found:") + path));
    }
    return String("");
}
/**
 * Skips any whitespace characters in the input file.
 *
 * @param f The input file to skip whitespace in.
 */
void SimpleJsonParser::_skipWhiteSpace(File f)
{
    if (f)
    {
        char c= (char)f.peek();
        while (c  == ' ' || c == '\n' || c == '\r' || c == '\t')
        {
           f.read();
           c= (char)f.peek();
        }
    }
}

/**
 * Skips whitespace characters in a given JSON text.
 *
 * @param jsontext The JSON text to skip whitespace in.
 * @param frompos The starting position to skip whitespace from.
 *
 * @return The position after skipping whitespace.
 *
 */
int SimpleJsonParser::_skipWhiteSpace(String jsontext, unsigned int frompos)
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
    /**
     * Retrieves the number of entries in a JSON file located at the specified path.
     *
     * @param path The path to the JSON file.
     *
     * @return The number of entries in the JSON file, or -1 if the file does not exist or there is a serious problem with SPIFFS.
     *
     */
int SimpleJsonParser::getNumberOfEntriesFromFile(String path)
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
            _SIMPLEJSON_PL(String(F("File open failed.") + path));
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
    else
    {
        _SIMPLEJSON_PL(String(F("File not found:") + path));
    }
    return -1;
}

/**
 * Counts the number of entries in a JSON string.
 *
 * @param jsontxt The JSON string to count the entries from.
 *
 * @return The number of entries in the JSON string.
 *
 */
int SimpleJsonParser::getNumberOfEntriesFromString(String jsontxt)
{
    int count = 0;
    for (unsigned int i = 0; i < jsontxt.length(); i++)
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

/**
 * Retrieves the key at the specified index from a JSON file.
 *
 * @param path The path to the JSON file.
 * @param index The index of the key to retrieve.
 *
 * @return The key at the specified index, or an empty string if the file does not exist or the index is out of range.
 *
 */
String SimpleJsonParser::getKeybyIndexFromFile(String path, uint32_t index)
{
    uint32_t count = 0;
    uint32_t frstpt = 0, secpt = 0;
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
            _SIMPLEJSON_PL(String(F("File open failed.") + path));
            return String("");
        }

        while (f.available())
        {
            if ((((c = (char)f.read()) == ',' || (c == '{'))) && (!frstpt))
            {
                _skipWhiteSpace(f);
                if ((c = (char)f.read()) == '"')
                {
                    frstpt = f.position();
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
                secpt = f.position();
                if (secpt && frstpt && (f.available() >= 2))
                {
                    _skipWhiteSpace(f);
                    if ((c = (char)f.read()) == ':')
                    {
                        _skipWhiteSpace(f);
                        if ((c = (char)f.read()) == '"')
                        {
                            if (index == count)
                            {
                                f.seek(frstpt, SeekSet);
                                while (frstpt < secpt - 1)
                                {
                                    result += String((char)f.read());
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
    }
    else
    {
        _SIMPLEJSON_PL(String(F("File not found:") + path));
    }
    return String("");
}
/**
 * Retrieves a value from a JSON file at a given index.
 *
 * @param path the path to the JSON file
 * @param index the index of the value to retrieve
 *
 * @return the value at the specified index, or an empty string if the file
 *         does not exist or the index is out of range.
 *
 */
String SimpleJsonParser::getValuebyIndexFromFile(String path, uint32_t index)
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
            _SIMPLEJSON_PL(String(F("File open failed.") + path));
            return String("");
        }

        while (f.available())
        {
            if ((c = (char)f.read()) == '"')
            {
                newpt = f.position();
                _skipWhiteSpace(f);
                if ((c = (char)f.read()) == '}' && oldpt)
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
                    count++; // shouldn't be here
                }
                else if (c == ':')
                {
                    _skipWhiteSpace(f);
                    if ((c = (char)f.read()) == '"')
                    {
                        oldpt = f.position();
                    }
                }

                else if (c == ',' && oldpt)
                {
                    _skipWhiteSpace(f);
                    if ((c = (char)f.read()) == '"')
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
                        oldpt = 0;
                    }
                }
            }
        }
    }
    else
    {
        _SIMPLEJSON_PL(String(F("File not found:") + path));
    }

    return String("");
}
/**
 * Retrieves the key from the JSON string by the specified index.
 *
 * @param jsontxt The JSON string to extract the key from.
 * @param index The index of the key to retrieve.
 *
 * @return The extracted key from the JSON string. If the index is out of range or the JSON is corrupt, an empty string is returned.
 *
 */
String SimpleJsonParser::getKeybyIndexFromString(String jsontxt, int index)
{
    int count = 0;
    uint32_t frstpt = 0, secpt = 0;
    for (uint32_t i = 0; i < jsontxt.length(); i++)
    {
        if ((jsontxt.substring(i, i + 1) == "{" || jsontxt.substring(i, i + 1) == ",") && !frstpt)
        {
            i++;
            i = _skipWhiteSpace(jsontxt, i);
            if (jsontxt.substring(i, i + 1) == "\"")
            {
                frstpt = i;
                continue;
            }
            else
            {
                _SIMPLEJSON_PL("Corrupt json:{ or , must follow a \"");
                goto iloopend;
            }
        }

        if (jsontxt.substring(i, i + 1) == "\"")
        {
            secpt = i;
            if (secpt && frstpt && i < jsontxt.length() - 2)
            {
                i++;
                i = _skipWhiteSpace(jsontxt, i);
                if (jsontxt.substring(i, i + 1) == ":")
                {
                    i++;
                    i = _skipWhiteSpace(jsontxt, i);
                    if (jsontxt.substring(i, i + 1) == "\"")
                    {
                        if (index == count)
                        {
                            return jsontxt.substring(frstpt + 1, secpt);
                        }
                        count++;
                        frstpt = 0;
                        secpt = 0;
                    }
                }
            }
        }
    }
iloopend:
    return String("");
}

/**
 * Retrieves the value from the JSON string by the specified index.
 *
 * @param jsontxt The JSON string to extract the value from.
 * @param index The index of the value to retrieve.
 *
 * @return The extracted value from the JSON string.
 *
 */
String SimpleJsonParser::getValuebyIndexFromString(String jsontxt, int index)
{
    int count = 0;
    uint32_t oldpt = 0, newpt = 0;
    for (uint32_t i = 0; i < jsontxt.length(); i++)
    {
        if (jsontxt.substring(i, i + 1) == "\"")
        {
            newpt = i;
            i++;
            i = _skipWhiteSpace(jsontxt, i);
            if (jsontxt.substring(i, i + 1) == "}" && oldpt)
            {
                if (index == count)
                {
                    return jsontxt.substring(oldpt + 1, newpt);
                }
                count++; // shouldn't be here
            }
            else if (jsontxt.substring(i, i + 1) == ":")
            {
                i++;
                i = _skipWhiteSpace(jsontxt, i);
                if (jsontxt.substring(i, i + 1) == "\"")
                {
                    oldpt = i;
                }
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
                    oldpt = 0;
                }
            }
        }
    }
    return String("");
}
/**
 * Extracts keys and values from a JSON file given the file path.
 *
 * @param path The path to the JSON file.
 *
 * @return A vector of pairs containing the extracted keys and values.
 *
 */
std::vector<std::pair<String, String>> SimpleJsonParser::extractKeysandValuesFromFile(String path)
{
    std::vector<std::pair<String, String>> result;
    uint32_t frstpt = 0, secpt = 0, thipt = 0, forpt = 0;
    char c;
    if (!SPIFFS.begin())
    {
        // Serious problem
        _SIMPLEJSON_PL(F("SPIFFS Mount failed."));
        result.clear();
        return result;
    } // end if

    if (SPIFFS.exists(path))
    {
        File f = SPIFFS.open(path, "r");
        if (!f)
        {
            _SIMPLEJSON_PL(String(F("File open failed.") + path));
            result.clear();
            return result;
        }
        
        while (f.available())
        {
            if ((((c = (char)f.read()) == ',' || (c == '{'))) && (!frstpt))
            {
                _skipWhiteSpace(f);
                if((c = (char)f.read()) == '"')
                {
                    frstpt = f.position();
                    continue;
                }
                else
                {
                    _SIMPLEJSON_PL("Corrupt json:{ or , must follow a \" at position:" + String(f.position()));
                    result.clear();
                    return result;
                }
            }

            if (c == '"')
            {

                if (frstpt && secpt && thipt && !forpt )
                {
                    forpt = f.position();
                    String key = "";
                    String value = "";
                    f.seek(frstpt, SeekSet);
                    while (frstpt < secpt - 1)
                    {
                        key += String((char)f.read());
                        frstpt++;
                    }

                    f.seek(thipt, SeekSet);
                    while (thipt < forpt - 1)
                    {
                        value += String((char)f.read());
                        thipt++;
                    }
                    _SIMPLEJSON_PL(key+":"+value);
                    result.emplace_back(std::make_pair(key, value));
                    frstpt = 0;
                    secpt = 0;
                    thipt = 0;
                    forpt = 0;
                }
                else if (frstpt && !secpt && !thipt && !forpt && (f.available() >= 2))
                {
                    secpt = f.position();
                    _skipWhiteSpace(f);
                    if ((c = (char)f.read()) == ':')
                    {
                        _skipWhiteSpace(f);
                        if ((c = (char)f.read()) == '"')
                        {
                            thipt = f.position();
                        }
                    }
                }
            }
        }
    }
    else
    {
        _SIMPLEJSON_PL(String(F("File not found:") + path));
    }
    return result;
}
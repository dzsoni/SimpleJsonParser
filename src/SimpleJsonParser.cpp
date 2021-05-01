#include <SimpleJsonParser.h>


SimpleJsonParser::SimpleJsonParser()
{
}

SimpleJsonParser::~SimpleJsonParser()
{
}

String SimpleJsonParser::FileToString(String path)
{
    String content = "";

    // start spiff
    if (!SPIFFS.begin())
    {
        // Serious problem
        Serial.println(F("SPIFFS Mount failed"));

        return String("");
    } //end if

    //Serial.println(F("SPIFFS Mount succesfull"));

    // read file
    if (SPIFFS.exists(path))
    {
        File f = SPIFFS.open(path, "r");
        if (!f)
        {
            Serial.println(F("file open failed"));
            return String("");
        } //end if

        // read file

        while (f.available())
        {
            content += char(f.read());
        }
        f.close();
        return content; //f.readString();
    }
    return String("");
}

String SimpleJsonParser::GetJSONValueByKey(String text, String key)
{
    if (text.length() == 0)
    {
        return String("");
    }

    String searchPhrase = String("\"");
    searchPhrase.concat(key);
    searchPhrase.concat("\":\"");
    int fromPosition = text.indexOf(searchPhrase, 0);
    if (fromPosition == -1)
    {
        // return because there is no status or it's null
        return String("");
    }

    fromPosition = fromPosition + searchPhrase.length();
    int toPosition = text.indexOf("\"", fromPosition);

    if (toPosition == -1)
    {
        // return because there is no end quote
        return String("");
    }
    text.remove(toPosition);
    return text.substring(fromPosition);
}
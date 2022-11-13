# SimpleJsonParser
It's for ESP8266,ESP32 or Arduino devices.
With SimpleJsonParser you can only parse json that look like this:
```
{ "key1":"value1",
  "key2":"value2",
  "key3" : "value3"}
```
Json can be string or SPIFFS file.

The values must be strings!

# Functions

You can find out how many entities are involved.
```
int    getNumberOfEntriesFromString(String jsontxt);
int    getNumberOfEntriesFromFile(String path);
```
You can get the value by key:
```
String getJSONValueByKeyFromFile(String path, String key);
String getJSONValueByKeyFromString(String text, String key);
```
You can get the value by index:
```
String getJSONValuebyIndexFromFile(String path,int index);
String getJSONValuebyIndexFromString(String jsontxt,int index);
```
You can get the key by index:
```
String getJSONKeybyIndexFromFile(String path, int index);
String getJSONKeybyIndexFromString(String jsontxt, int index);
```

#include "storage.h"
#include "SD.h"
#include "inttypes.h"

uint8_t storage_SD_loaded = 0;
String currentWorkingFile = "";

uint8_t storageBeginSD(){
    if (!SD.begin(5)) {
        Serial.println("No SD card found.");
        return 0;
    }
    Serial.println("SD card found.");
    storage_SD_loaded = 1;
    return 1;
}

static String readLine(File dataFile, uint16_t lineNumber){
    uint16_t currentLine = 0;
    String line = "";
    do {
        line = dataFile.readStringUntil('\n');
        currentLine++;
        Serial.println(currentLine);
    } while(currentLine != lineNumber);
    Serial.println(line);
    return line;
}

String storageReadLine(const String &fileName, uint16_t lineNumber){

    File dataFile;
    dataFile = SD.open(fileName);

    uint16_t currentLine = 0;
    String line = "";
    do {
        line = dataFile.readStringUntil('\n');
        currentLine++;
    } while(currentLine != lineNumber);
    dataFile.close();
    return line;
}

uint8_t storageWriteLine(const String &fileName, uint16_t lineNumber, char * data){
    uint16_t currentLine = 0;
    String line = "";
    File dataFile;
    dataFile = SD.open(fileName, O_WRITE | O_CREAT | O_TRUNC);
    while(currentLine != lineNumber) {
        line = dataFile.readStringUntil('\n');
        if (line == ""){
            return 0;
        }
        currentLine++;
    }
    dataFile.write(data);
    dataFile.close();

    return 1;

}

static void writeIntLine(File dataFile, uint16_t lineNumber, uint32_t data){
    Serial.println(data);
    uint16_t currentLine = 1;
    String line = "";
    while(currentLine != lineNumber) {
        line = dataFile.readStringUntil('\n');
        currentLine++;
    }
    char temp[6];
    sprintf(temp,"%6d\n", data);
    dataFile.write(temp);
}

uint8_t storageWriteToFile(const String &fileName, uint32_t data){
    if(!SD.exists(fileName))
        return 0;
    File dataFile = SD.open(fileName, FILE_WRITE);
    char temp[6];
    sprintf(temp,"%6d\n", data);
    dataFile.write(temp);
    dataFile.close();
    return 1;
}

void storageNewFile(){
    currentWorkingFile = "";
    for (uint32_t &i : parameterList) {
        i =0;
    }
    for (uint8_t &i : booleanList) {
        i = 0;
    }
}

uint8_t storageLoadSD(const String &fileName){
    Serial.println(SD.exists(fileName));
    if(SD.exists(fileName)){
        File dataFile = SD.open(fileName);
        for (uint32_t &i : parameterList){
            //Serial.println(dataFile.readStringUntil('\n').toInt());
            i = (uint16_t) dataFile.readStringUntil('\n').toInt();
        }
        for (uint8_t &i : booleanList) {
            i = (uint8_t) dataFile.readStringUntil('\n').toInt();
        }
        dataFile.close();
    }
    return 1;
}

uint8_t storageSaveParameters() {
    if (storage_SD_loaded == 0u) {
        Serial.println("First if");
        if (storageBeginSD() == 0u)
            return 0;
    }
    char temp[12];

    if (currentWorkingFile != "") {
        currentWorkingFile.toCharArray(temp, 12);
    }
    else {
        uint8_t current_file_number = 0;
        String lineData;
        /*if (SD.exists("SYSTEM_1.VAR")) {
            File dataFile = SD.open("SYSTEM_1.VAR", O_WRITE | O_CREAT | O_TRUNC);
            lineData = readLine(dataFile, 1);
            Serial.println("Got line");
            current_file_number = (uint8_t) lineData.toInt();
            writeIntLine(dataFile, 1, current_file_number+1h);
            dataFile.close();
        }*/
        Serial.println("testing");
        sprintf(temp, "%08d.DAT", current_file_number+1);
    }
    Serial.println(temp);
    File dataFile = SD.open(temp, O_WRITE | O_CREAT | O_TRUNC);
    Serial.println("yeah");
    for (uint16_t i : parameterList) {
        char currentParam[6];
        sprintf(currentParam, "%06d", i);
        Serial.println("test");
        dataFile.write(currentParam);
        dataFile.write('\n');
    }
    for (uint8_t i : booleanList) {
        char currentParam[6];
        sprintf(currentParam, "%06d", i);
        dataFile.write(currentParam);
        dataFile.write('\n');
    }

    currentWorkingFile = String(temp);
    dataFile.close();
    Serial.println(SD.exists(temp));

    return 1;
}

uint8_t storageGetFiles(Array * listOfFiles){
    if (storage_SD_loaded == 0u) {
        if (storageBeginSD() == 0u)
            return 0;
    }

    File dir = SD.open("/");
    Serial.println(dir.name());
    String tempString;
    while (true) {
        File entry =  dir.openNextFile();
        if (! entry) {
            // no more files
            break;
        }
        Serial.print(entry.name());
        if (!entry.isDirectory()) {
            // files have sizes, directories do not
            tempString += entry.name();
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
    uint16_t string_length = tempString.length();
    Serial.println(string_length);
    String string_array;
    uint8_t current_file = 0;
    for (uint16_t i = 0; i < string_length; ++i) {
        insertArray(listOfFiles, (char) tempString[i]);
        if(((i+1)%12) == 0){
            current_file++;
        }
    }

    return current_file;
}

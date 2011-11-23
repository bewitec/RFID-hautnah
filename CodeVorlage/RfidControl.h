//Copyright 2011 INKA - HTW Berlin
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.


#ifndef _RFIDCONTROL_H_
#define _RFIDCONTROL_H_

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip> 

struct is_id {
    bool operator ()(const std::string &str) {
        return !str.empty() && str.length() != 16;
    }
};

struct is_data {
    bool operator ()(const std::string &str) {
        return !str.empty() && str.compare(0,2,"00");
    }
};

class RfidControl 
{
public:
    //RfidControl();
    bool connect(std::string);
    bool disconnect();
    std::vector<std::string> readTags();
    std::string readTagData(std::string);
    bool writeTagData(std::string, std::string);
    std::vector<std::string> writeMetraTecCommand(std::string);
    std::string readNdefMessage(std::string);
    bool writeNdefMessage(std::string, std::string);
protected:
    std::string device;
    int fileDescriptor;
private:
    void splitString(std::string, std::vector<std::string>*);
    std::string byteToHexString(unsigned char);
    std::string charStringToHexString(std::string);
    std::string hexStringToCharString(std::string);
    bool writeDataToReader(std::string, std::string, int);
    
};

#endif

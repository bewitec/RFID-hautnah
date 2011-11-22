// copyright - 2011 INKA - HTW Berlin


#include "RfidControl.h"
#include "SerialDevice.c"


bool RfidControl::connect(std::string dev){
    //connect Code
    return false;
}

bool RfidControl::disconnect(){
    //disconnect Code
    return false;
}

std::vector<std::string> RfidControl::writeMetraTecCommand(std::string command){
    //write command direct to reader
    std::vector<std::string> results;
    return results;
    
}

std::vector<std::string> RfidControl::readTags(){
    //read Tags and return vector with ID-strings
}



std::string RfidControl::readTagData(std::string id){
    std::string result;
    //read Tag data for Tag with id and return string of content
    return result;
    
}


bool RfidControl::writeTagData(std::string data, std::string id){
    //write data on Tag with id and return true on success
    return true;
}


//helper functions
void RfidControl::splitString(std::string tosplit, std::vector<std::string>* results){
        int found;
        found = tosplit.find_first_of("\\r");
        while(found != std::string::npos){
            if(found > 0){
                results->push_back(tosplit.substr(0,found));
            }
            tosplit = tosplit.substr(found+2);
            found = tosplit.find_first_of("\\r");
        }
        if(tosplit.length() > 0){
            results->push_back(tosplit);
        }
    
}

std::string RfidControl::charStringToHexString(std::string str) {
    std::string returnValue;
    for(std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        returnValue.append(byteToHexString(*i));
    }
    return returnValue;
}

std::string RfidControl::byteToHexString(unsigned char c){
    std::ostringstream out;
    out << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(c));
    return out.str();
}

std::string RfidControl::hexStringToCharString(std::string str){
    std::ostringstream outputStream;
    for(size_t i = 0; i + 1 <= str.size(); i += 2){
        unsigned short s;
        std::istringstream(str.substr(i, 2)) >> std::hex >> s;
        outputStream << static_cast<unsigned char>(s);
    }
    return outputStream.str();
}
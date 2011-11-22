// copyright - 2011 INKA - HTW Berlin


#include "RfidControl.h"
#include "SerialDevice.c"



bool RfidControl::connect(std::string dev){
    device = dev;
    std::string result;
    fileDescriptor = OpenSerialDevice(device.c_str());
    
    //no file desc
    if (fileDescriptor <0) return false;
    
    //SET MOD 156
    result = WriteCommand(fileDescriptor, "MOD 156\r");
    if(result.find("OK!")==std::string::npos) {
        CloseSerialDevice(fileDescriptor);
        return false;
    }
    
    // Icode settings
    result = WriteCommand(fileDescriptor, "SRI SS 100 \r");
    if(result.find("OK!")==std::string::npos) {
        CloseSerialDevice(fileDescriptor);
        return false;
    }
    return true;
}

bool RfidControl::disconnect(){
    CloseSerialDevice(fileDescriptor);
    return true;
}

std::vector<std::string> RfidControl::writeMetraTecCommand(std::string command){
    command.append("\r");
    std::string answer = WriteCommand(fileDescriptor,command.c_str());
    std::vector<std::string> results;
    splitString(answer, &results);
    return results;
    
}


std::vector<std::string> RfidControl::readTags(){
    std::vector<std::string> ids;
    std::string result = WriteCommand(fileDescriptor, "INV\r");
    splitString(result, &ids);
    ids.erase( std::remove_if( ids.begin(), ids.end(), is_id() ), ids.end() );
    return ids;
}



std::string RfidControl::readTagData(std::string id){
    std::string result;
    int blockNr=0;
    int numOfTrys=0;
    std::string error = "Error: ";
    do {
        // create read command for metraTec reader
        std::string command="REQ 2220";
        command.append(id);
        command.append(byteToHexString(blockNr));
        command.append(" crc\r");
        
        
        std::string answer;
        do {
            if(numOfTrys>3) return error.append(answer.substr(0,3));
            answer = WriteCommand(fileDescriptor, command.c_str());
            numOfTrys++;
        } while ( answer.find("00")==std::string::npos);                
        // send command to reader and parse answer
        std::vector<std::string> results;
        splitString(answer, &results);

        results.erase( std::remove_if( results.begin(), results.end(), is_data() ), results.end() );
        
        if(results.size()!=1) return "nothing to read";
        
        std::string blockConntent = results[0].substr(2, results[0].length()-6);
        if (blockConntent.find("00000000")!= std::string::npos) return result;
        result.append(hexStringToCharString(blockConntent));
        blockNr++;
        numOfTrys=0;
    } while (blockNr<27);
    return result;
    
}


bool RfidControl::writeTagData(std::string data, std::string id){
    int numOfTrys =0;
    while (data.length()%4!=0) {
        data.append(" ");
    }
    int blocksToWrite = data.length()/4;
    
    
    if(blocksToWrite>26) {
        std::cout << "error wihle writing: content to big for tag" << std::endl;
        return false;
        
    }
    int blockNum =0;
    do {
        std::string command="REQ 2221";
        command.append(id);
        command.append(byteToHexString(blockNum));
        if(blocksToWrite==0) command.append("00000000");
        else command.append(charStringToHexString(data.substr(blockNum*4,4)));
        command.append(" crc\r");
        
        std::string answer;
        do {
            if(numOfTrys>3) {
                std::cout << "Error: " << answer.substr(0,3) << std::endl; 
                return false;
            }
            answer = WriteCommand(fileDescriptor, command.c_str()); 
            numOfTrys++;
        } while ( answer.find("00")==std::string::npos);                
        
        blockNum++;
        blocksToWrite--;
        numOfTrys=0;
    } while (blocksToWrite>=0);
    return true;
}



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


//optional Read NDEF Messages:
std::string RfidControl::readNdefMessage(std::string id){
        std::string result ="";
        int numOfTrys=0;
        //read first block
        //check if it has NDEF CC File (e1400e01)
        std::string command="REQ 2220";
        command.append(id);
        command.append("00 crc\r");
        std::string answer;
        do {
            if(numOfTrys>3) return "no ndef messages present";
            answer = WriteCommand(fileDescriptor, command.c_str()); 
            numOfTrys++;
        } while ( answer.find("E1400E01")==std::string::npos);
        numOfTrys =0;
        
        //read third block (check for ndef lenght)
        command.replace(24, 2, "02");
        do {
            if(numOfTrys>3) return "error while reading";
            answer = WriteCommand(fileDescriptor, command.c_str()); 
            numOfTrys++;
        } while ( answer.find("00")==std::string::npos);
        numOfTrys =0;
        int length;
    
        // get ndef lenght !!!!!
        std::stringstream ss;
        ss << std::hex << answer.substr(7,2);
        ss >> length;
    
        //last byte on block third is content
        result = hexStringToCharString(answer.substr(13,2));
        int blockCount = ((length-1)+3)/4;
        
        //get rest of ndef message
        command="REQ 2223";
        command.append(id);
        command.append("03");
        command.append(byteToHexString(blockCount));
        command.append(" crc\r");
        do {
            if(numOfTrys>3) return "error while reading";
            answer = WriteCommand(fileDescriptor, command.c_str()); 
            numOfTrys++;
        } while ( answer.find("00")==std::string::npos);
        numOfTrys =0;         
        result.append(hexStringToCharString(answer.substr(7, blockCount*8)));
    
    return result;
}

bool RfidControl::writeNdefMessage(std::string id, std::string data){
        int numOfTrys;
        std::string dataHex = charStringToHexString(data);
        int length =dataHex.length();
        if(length<=0)return false;
        int anzahlBlocks = ((length-2)+7)/8;
        int currentBlock=0;
        std::string dataC;
        int posLocation=0;
        int posLength=2;
        while (currentBlock<anzahlBlocks+3) {
            switch (currentBlock) {
                case 0:
                    //write NDEF CC File
                    if (!writeDataToReader(id, "e1400e01", currentBlock)) return false;
                    currentBlock++;
                    break;
                case 1:
                    //write TLV and size of NDEF Message
                    dataC = "03";
                    dataC.append(byteToHexString(length+4));
                    dataC.append("d101");
                    if (!writeDataToReader(id, dataC, currentBlock)) return false;
                    currentBlock++;
                    break;
                case 2:
                    //write URI length 
                    dataC=byteToHexString(length/2+1);
                    dataC.append("5500");
                    dataC.append(dataHex.substr(posLocation,posLength));
                    posLength=8;
                    posLocation=2;
                    if (!writeDataToReader(id, dataC, currentBlock)) return false;
                    currentBlock++;
                    break;
                default:
                    //write URI content
                    if (posLocation+posLength>length) {
                        int tofill = posLocation+posLength - length;
                        if (tofill==2) dataHex.append("00");
                        if (tofill==4) dataHex.append("0000");
                        if (tofill==6) dataHex.append("000000");
                    }
                    if (!writeDataToReader(id, dataHex.substr(posLocation,posLength), currentBlock)) return false;
                    posLocation +=8;
                    currentBlock++;
                    break;
            }
        }
    return true;
    
}

bool RfidControl::writeDataToReader(std::string id, std::string data, int blockNr){
        int numOfTrys=0;
        std::string command="REQ 2221";
        command.append(id);
        command.append(byteToHexString(blockNr));
        command.append(data);
        command.append(" crc\r");
        std::string answer;
        do {
            if(numOfTrys>5) {
               std::cout << "Error: " << answer.substr(0,3) << std::endl;
               return false; 
            }
            answer = WriteCommand(fileDescriptor, command.c_str()); 
            numOfTrys++;
        } while ( answer.find("00")==std::string::npos);
    return true;
    
}
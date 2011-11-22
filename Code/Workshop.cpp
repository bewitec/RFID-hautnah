// copyright - 2011 INKA - HTW Berlin


#include "RfidControl.h"
#include <cstdlib>
#include <signal.h>

void connect(RfidControl*);
void readTags(RfidControl*);
void readTagsContinuously(RfidControl*);
void readTagData(RfidControl*);
void writeTagData(RfidControl*);
void disconnect(RfidControl*);
void writeCommand(RfidControl*);
void signalFunction(int);
void help();

static bool connected = false;
static bool contRead=false;

using namespace std;
 main()
{
    RfidControl controler;
    system("clear");
    cout << "Willkommen zum Workshop \"RFID hautnah\"" << endl; 
    string command;
    do {
        cout << ">> ";
        getline (cin, command);
        if(command =="c" || command == "connect"){
             connect(&controler);
        }else if(command=="r" || command == "read"){
            if (!connected){
                cout << "Keine Verbindung zum Reader" << endl;
                cout << "'c' oder 'connect'" << endl;
            }
            
            else readTags(&controler);
        }else if(command=="rc" || command == "readcont"){
            if (!connected){
                cout << "Keine Verbindung zum Reader" << endl;
                cout << "'c' oder 'connect'" << endl;
            }
            
            else readTagsContinuously(&controler);
        }else if(command =="rd" || command == "readdata"){
            if (!connected) {
                cout << "Keine Verbindung zum Reader" << endl;
                cout << "'c' oder 'connect'" << endl;
            }
            else readTagData(&controler);
        }else if(command =="w" || command == "write"){
            if (!connected) {
                cout << "Keine Verbindung zum Reader" << endl;
                cout << "'c' oder 'connect'" << endl;
            }
            else writeTagData(&controler);
        }else if(command == "?" || command == "help"){
            help();
        }else if(command == "clear"){
            system("clear");
        }else if(command == "wc" || command=="writecommand"){
            if (!connected) {
                cout << "Keine Verbindung zum Reader" << endl;
                cout << "'c' oder 'connect'" << endl;
            }
            else writeCommand(&controler);
        }else if(command == "d" || command=="disconnect"){
            if (!connected) {
                cout << "Keine Verbindung zum Reader" << endl;
                cout << "'c' oder 'connect'" << endl;
                }
                else disconnect(&controler);        
        }else if (command.length()!=0 && command != "q" && command != "Q" && command != "quite") {
            cout << "Befehl nicht gefunden" << endl;
            cout << "'?' oder 'help' fuer Befehlsuebersicht" << endl;
        }
        
        
    } while (command != "q" && command != "Q" && command != "quite");
    cout << "Aufwiedersehen!" << endl;   
    if(connected) disconnect(&controler);
}

void connect(RfidControl* cont){
    std::string path;
    cout << "Bitte geben Sie Devicepfad vom metraTec Reader ein:" << endl;
    getline (cin, path);
    if(cont->connect(path)){
        std::cout << "Verbindung zum Reader wurde erfolgreich aufgebaut" << std::endl;
        connected = true;
    }else{
        std::cout << "Verbindung zum Reader ist fehlgeschlagen" << std::endl;
    }
    
}

void readTags(RfidControl* cont){
    std::vector<std::string> ids = cont->readTags();
    if(ids.empty()) {
        cout << "Keine Tags in Reichweite" << endl;
    }
    else{
        std::vector<std::string>::iterator it; 
        std::cout << "Folgende Tags wurden gefunden: " << std::endl;
        for(it = ids.begin(); it < ids.end(); it++){
            std::cout << "ID: " << *it <<std::endl;
        }
    }
    
}

void readTagsContinuously(RfidControl* cont){
    //code for continously read tag id's
    //use while contRead true and use sleep ;)
}



void readTagData(RfidControl* cont){
    std::string id;
    cout << "Bitte geben Sie die ID des Tags ein:" << endl;
    cout << "'r' oder 'read' listet alle Tags auf" << endl;
    getline (cin, id);
    string answer = cont->readTagData(id);
    cout << "Inhalt des Tags:" << endl;
    cout << answer << endl;
}


void writeTagData(RfidControl* cont){
    std::string id;
    cout << "Bitte geben Sie die ID des Tags ein:" << endl;
    cout << "'r' oder 'read' listet alle Tags auf" << endl;
    getline (cin, id);
    std::string data;
    cout << "Bitte geben Sie die zu schreibenden Daten ein:" << endl;
    getline (cin, data);
    if(cont->writeTagData(data,id)) cout << "Schreibvorgang war erfolgreich" << endl;
    else cout << "Schreibvorgang ist fehlgeschlagen" << endl;
}

void writeCommand(RfidControl* cont){
    std::string command;
    cout << "Bitte geben Sie den zu schreibenden Command ein: (ohne <CR> '\\r')" << endl;
    getline (cin, command);
    vector<string> answer = cont->writeMetraTecCommand(command);
    std::vector<std::string>::iterator it; 
    std::cout << "Antwort des Readers: " << std::endl;
    for(it = answer.begin(); it < answer.end(); it++){
        std::cout  << *it <<std::endl;
    }
}
void disconnect(RfidControl* cont){
    
    cont->disconnect();
    cout << "Verbindung zum Reader wurde geschlossen" << endl;
    connected = false;
}

void help(){
    system("clear");
    cout << "Folgende Befehle sind verfuegbar:" << endl;
    cout << "q,  quite                     "<< "Programm beenden" << endl;
    cout << "c,  connect                   "<< "Verbindung zum metraTec Reader aufbauen" << endl;
    cout << "d,  disconnect                "<< "Verbindung zum metraTec Reader abbrechen" << endl;
    cout << "r,  read                      "<< "Lesen aller ID's der verfuegbaren Tags" << endl;
    cout << "rc, readcont                  "<< "Lesen aller ID's der verfuegbaren Tags in einer Endlosschleife" << endl;
    cout << "rd, readdata                  "<< "Lesen des Inhaltes eines Tags" << endl;
    cout << "wd, write                     "<< "Daten auf einen Tag schreiben" << endl;
    cout << "wc, writecommand              "<< "Schreiben eines Commands direkt auf den Reader" << endl;
    cout << "clear                         "<< "Bildschirm loeschen" << endl;
    cout << "?, help                       "<< "Diese Hilfe wird angezeigt" << endl;
}


void signalFunction(int x){
    //catch singal from user and set contRead false
}
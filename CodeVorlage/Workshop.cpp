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
void readNdef(RfidControl*);
void writeNdef(RfidControl*);
void signalFunction(int);
void help();

static bool connected = false;
static bool contRead=false;

using namespace std;
 main()
{
    signal(SIGTSTP,signalFunction);
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
        }else if(command == "rndef" || command=="readndef"){
            if (!connected) {
                cout << "Keine Verbindung zum Reader" << endl;
                cout << "'c' oder 'connect'" << endl;
            }
            else readNdef(&controler);    
        }else if(command == "wndef" || command=="writendef"){
            if (!connected) {
                cout << "Keine Verbindung zum Reader" << endl;
                cout << "'c' oder 'connect'" << endl;
            }
            else writeNdef(&controler); 
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
    cout << "Abbrechen mit Crtl-Z" << endl;
    std::vector<std::string> ids_alt;
    std::vector<std::string> ids_neu;
    contRead=true;
    while (contRead) {
        std::vector<std::string> ids_neu = cont->readTags();
        if(ids_alt.size()>ids_neu.size()){
            std::vector<std::string>::iterator it; 
            for(it = ids_alt.begin(); it < ids_alt.end(); it++){
                std::vector<std::string>::iterator findIt = find(ids_neu.begin(),ids_neu.end(), *it);
                if (findIt== ids_neu.end()) {
                    cout << "- ID: " << *it << endl;
                }
            }
        }else if(ids_alt.size()<ids_neu.size()){
            std::vector<std::string>::iterator it; 
            for(it = ids_neu.begin(); it < ids_neu.end(); it++){
                std::vector<std::string>::iterator findIt = find(ids_alt.begin(),ids_alt.end(), *it);
                if (findIt== ids_alt.end()) {
                    cout << "+ ID: " << *it <<  endl;
                }
            } 
        }
        ids_alt =ids_neu;
        sleep(1);
    }
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

void readNdef(RfidControl* cont){
    std::string id;
    cout << "Bitte geben Sie die ID des Tags ein:" << endl;
    cout << "'r' oder 'read' listet alle Tags auf" << endl;
    getline (cin, id);
    string answer = cont->readNdefMessage(id);
    cout << "NDEF Message auf dem Tag:" << endl;
    cout << answer << endl;
}

void writeNdef(RfidControl* cont){
    std::string id;
    cout << "Bitte geben Sie die ID des Tags ein:" << endl;
    cout << "'r' oder 'read' listet alle Tags auf" << endl;
    getline (cin, id);
    std::string data;
    cout << "Bitte geben Sie die zu schreibenden NDEF URI ein:" << endl;
    getline (cin, data);
    if(cont->writeNdefMessage(id,data)) cout << "Schreibvorgang war erfolgreich" << endl;
    else cout << "Schreibvorgang ist fehlgeschlagen" << endl;
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
    cout << "rndef, readndef               "<< "Lese NDEF Message (URI) eines Tags" << endl;
    cout << "wndef, writendef              "<< "Schreibe eine NDEF Message (URI) auf einen Tag" << endl;
    cout << "clear                         "<< "Bildschirm loeschen" << endl;
    cout << "?, help                       "<< "Diese Hilfe wird angezeigt" << endl;
}


void signalFunction(int x){
    cout << "Enlosschleife beenden..." << endl;
    contRead=false;
}
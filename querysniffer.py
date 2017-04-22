from scapy.all import *
import socket

spf_ip = '192.95.56.132'
spf_port = 7878

def pkt_callback(pkt):
    if pkt.getlayer(IP) and pkt.getlayer(UDP) and pkt.getlayer(Raw):
        payload = pkt[Raw].load
        if payload.startswith('SAMP'):
            sendUDP(spf_ip,spf_port,'%s|%d' % (pkt[IP].src,pkt[UDP].sport))


def sendUDP(dstIp, dstPort, payload):
    send(IP(dst=dstIp) / UDP(dport=dstPort) / Raw(payload))
    #sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    #sock.connect((dstIp,dstPort))
    #sock.send(payload)
    #sock.close()
    print payload

sniff(iface="ens18", prn=pkt_callback, filter="udp and dst port 7777", store=0)

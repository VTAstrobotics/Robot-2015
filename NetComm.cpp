/*
 * NetComm.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: Anirudh Bagde
 */

#include "NetComm.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "crc-16.h"

NetComm::NetComm() {
    recvSock = socket(AF_INET, SOCK_DGRAM, 0);
    int reuse = 1;
    setsockopt(recvSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(NETCOMM_RECVPORT);
    bind(recvSock, (sockaddr*) &addr, sizeof(addr));
    fcntl(recvSock, F_SETFL, O_NONBLOCK);
    bzero(&ifr, sizeof(ifr));
    strcpy(ifr.ifr_name, "wlan0");
}

NetComm::~NetComm() {
    close(recvSock);
}

bool NetComm::getData(ControlData* data) {
    CommData rawData;
    int size = sizeof(CommData);
    bzero(data, sizeof(ControlData));
    bzero(&rawData, size);
    int len = recvfrom(recvSock, &rawData, size, 0, NULL, NULL);
    if (len < size) {
        return false;
    }
    unsigned short crc16Check = crc16((unsigned char*) &rawData,
            sizeof(CommData) - sizeof(unsigned short));
    if(crc16Check != rawData.crc16) {
        return false;
    }
    data->id = rawData.id;
    data->val = rawData.val;
    unsigned int dpad = rawData.val;
    if(dpad & (1<<6)) {
        data->dpadY = 1;
    } else if(dpad & (1<<4)) {
        data->dpadY = -1;
    } else if(dpad & (1<<2)) {
        data->dpadX = 1;
    } else if(dpad & 1) {
        data->dpadX = -1;
    }
    return true;
}

bool NetComm::isNetworkUp() {
    if(ioctl(recvSock, SIOCGIFFLAGS, &ifr) != -1) {
        int check_flags = IFF_UP | IFF_RUNNING;
        return (ifr.ifr_flags & check_flags) == check_flags;
    }
    return false;
}

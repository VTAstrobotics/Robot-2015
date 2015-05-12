/*
 * NetComm.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: Anirudh Bagde
 */

#include "NetComm.h"
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "crc-16.h"

NetComm::NetComm() :
        pingSent(false), pingTime(0.0) {
    recvSock = socket(AF_INET, SOCK_DGRAM, 0);
    int reuse = 1;
    setsockopt(recvSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(RECVPORT);
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
    if(len < size) {
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
    if(dpad & (1 << 6)) {
        data->dpadY = 1;
    } else if(dpad & (1 << 4)) {
        data->dpadY = -1;
    } else if(dpad & (1 << 2)) {
        data->dpadX = 1;
    } else if(dpad & 1) {
        data->dpadX = -1;
    }
    return true;
}

bool NetComm::isNetworkUp() {
    // Make sure interface is up
    if(ioctl(recvSock, SIOCGIFFLAGS, &ifr) != -1) {
        int check_flags = IFF_UP | IFF_RUNNING;
        if((ifr.ifr_flags & check_flags) != check_flags) {
            return false;
        }
    }
    // Ping the driver station every so often
    double elapsed = getTime() - pingTime; // time since last ping was sent
    if(pingSent) {
        return elapsed < PING_WAIT; // return true until it times out
    } else if(elapsed > PING_INTERVAL) {
        // TODO send ping
    }
    return true; // assume it's fine for now
}

double NetComm::getTime() {
    timespec time;
    bzero(&time, sizeof(time));
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec + (time.tv_nsec / 1.0e9);
}

/*
 * NetComm.h
 *
 *  Created on: Mar 6, 2015
 *      Author: Anirudh Bagde
 */

#ifndef ROBOT_2015_NETCOMM_H_
#define ROBOT_2015_NETCOMM_H_

#include <stdint.h>
#include <net/if.h>

// Little-endian on Galileo (x86)
struct __attribute__((__packed__)) CommData {
    uint8_t id;
    uint8_t val;
    uint16_t crc16;
};

struct ControlData {
    uint8_t id;
    uint8_t val;
    int8_t dpadY;
    int8_t dpadX;
};

class NetComm {
public:
    static const int RECVPORT = 6800;
    static const int PING_INTERVAL = 3000;
    static const int PING_WAIT = 250;

    NetComm();
    ~NetComm();

    bool getData(ControlData* data);
    bool isNetworkUp();

private:
    static double getTime();

    int recvSock;
    ifreq ifr; // For detecting if network interface is up
    bool pingSent;
    double pingTime;

};

#endif /* ROBOT_2015_NETCOMM_H_ */

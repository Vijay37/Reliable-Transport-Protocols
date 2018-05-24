#include "../include/simulator.h"
#include <cstring>
#include <iostream>
using namespace std;
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
int winSize = 0;
int base =0;
int nexSeqNum=0;
struct pkt buffer[1000];
int bufferPointer=0;
int expectedSeqNum=0;
int bufLen = 0;
struct pkt gloPkt,gloSndPkt;
float timer = 50;
int lastAcked=0;
bool readyToSend=true;
bool winSizeExc=false;
float startTime;
float endTime;
void sendPacket(struct pkt packet,int seqNum);
void A_output(struct msg message)
{
    strcpy(gloPkt.payload,message.data);
    buffer[bufLen] = gloPkt;
    bufLen++;
    if(nexSeqNum < base+winSize){
        winSizeExc=false;
        gloSndPkt=buffer[nexSeqNum];
        sendPacket(gloSndPkt,nexSeqNum);
        if(base==nexSeqNum) {
            startTime=get_sim_time();
            starttimer(0, timer);
        }
        nexSeqNum++;
    }
    else{
        winSizeExc=true;
    }
}
void sendPacket(struct pkt packet,int seqNum){
    int checksum=0;
    packet.seqnum=nexSeqNum;
    packet.acknum=0;
    checksum+=packet.acknum+packet.seqnum;
    for(int index=0;index<20;index++)
        checksum+=packet.payload[index];
    packet.checksum=checksum;
    buffer[nexSeqNum]=packet;
    tolayer3(0,packet);
}
/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
    int checksum=0;
    checksum+=packet.seqnum+packet.acknum;
    endTime = get_sim_time();
//    timer=(timer+endTime-startTime)/2;
    if(packet.checksum==checksum) {
        base = packet.acknum + 1;
        if (base == nexSeqNum) {
            stoptimer(0);
        }
    }
    else {
        stoptimer(0);
        starttimer(0,timer);
    }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    readyToSend = false;
    starttimer(0,timer);
    startTime=get_sim_time();
    for(int index=base;index<nexSeqNum;index++){
        tolayer3(0,buffer[index]);
    }
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
winSize=getwinsize();
timer=5*winSize;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    int checksumB=0;
    checksumB+=packet.acknum+packet.seqnum;
    struct pkt ackPkt;
    for(int index=0;index<20;index++)
        checksumB+=packet.payload[index];
    if(checksumB==packet.checksum) {
        if (packet.seqnum == expectedSeqNum) {
            lastAcked=packet.seqnum;

            tolayer5(1, packet.payload);
            checksumB=0;
            ackPkt.seqnum=0;
            ackPkt.acknum = lastAcked;
            checksumB+=ackPkt.seqnum+ackPkt.acknum;
            ackPkt.checksum=checksumB;
            tolayer3(1, ackPkt);
            expectedSeqNum++;
        }

    }

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}

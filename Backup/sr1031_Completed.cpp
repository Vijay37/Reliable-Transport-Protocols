#include "../include/simulator.h"
#include <cstring>
#include <iostream>
#include <map>
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

float timers[100];
int timerLen=0;
int timerPointer=0;
int winSize = 0;
int base =0;
int nexSeqNum=0;
struct pkt buffer[1000];
struct pkt receiveBuffer[1000];
int bufferPointer=0;
int expectedSeqNum=0;
float timer=50;
int lastAcked=0;
map <float,int> timerMap;
struct pkt gloPkt,gloSndPkt;
map <int,int> ackStatus;
int bufLen = 0;
int checkTimers();
int latestAckReceived=0;
void A_output(struct msg message)
{

    strcpy(gloPkt.payload,message.data);
    buffer[bufLen] = gloPkt;
    bufLen++;
    if(nexSeqNum < base+winSize) {
//        cout<<"Timer Length : "
        int checksum=0;
        gloSndPkt=buffer[nexSeqNum];
        gloSndPkt.seqnum=nexSeqNum;
        gloSndPkt.checksum=1;
        gloSndPkt.acknum=0;
        checksum+=gloSndPkt.acknum+gloSndPkt.seqnum;
        for(int index=0;index<20;index++)
            checksum+=gloSndPkt.payload[index];
        gloSndPkt.checksum=checksum;
        buffer[nexSeqNum]=gloSndPkt;
        tolayer3(0,gloSndPkt);
        ackStatus[nexSeqNum]=0;
        if (timerLen == 0) {

            timers[timerLen] = get_sim_time();
            timerMap[timers[timerLen]] = nexSeqNum;

            timerLen = 1;
            starttimer(0, timer);
        } else {

            if (timerPointer == timerLen) {

                starttimer(0, timer);
            }
            timers[timerLen] = get_sim_time();
            timerMap[timers[timerLen]] = nexSeqNum;
            timerLen++;
            if(timerLen == 100) {
                timerLen = 1;
            }
        }
        nexSeqNum++;
    }

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
    int checksum=0;
    checksum+=packet.seqnum+packet.acknum;
    if(checksum!=packet.checksum)
        return;
    int ackNum=packet.acknum;
    if(ackNum>=latestAckReceived) {
        latestAckReceived = ackNum;
    }
    ackStatus[ackNum]=1;
    if(base==ackNum) {
        for(int i=base;i<=latestAckReceived;i++) {
            if(ackStatus[i]==0)
                break;
            base++;
        }
    }

}

int checkTimers(){
    timerPointer++;
    int prevTimer=99;
    if(timerPointer==100)
        timerPointer=1;
    else
        prevTimer=timerPointer-1;
    if(timerLen==timerPointer) {
        return timerMap[timers[prevTimer]];
    }

    float nxtTimerStartTime = timers[timerPointer];
    nxtTimerStartTime+=timer;
    float currTime = get_sim_time();
    starttimer(0,nxtTimerStartTime-currTime);
    return timerMap[timers[prevTimer]];
}
/* called when A's timer goes off */
void A_timerinterrupt()
{
    int pktSeqNo = checkTimers();
    struct pkt packet;
    if(ackStatus[pktSeqNo]==0) {
        if (timerPointer == timerLen) {

            starttimer(0, timer);
        }
        timers[timerLen] = get_sim_time();
        timerMap[timers[timerLen]] = pktSeqNo;
        timerLen++;
        if(timerLen == 100) {
            timerLen = 1;
        }
        tolayer3(0,buffer[pktSeqNo]);
        ackStatus[pktSeqNo]=0;
    }
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    winSize=getwinsize();
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
    if(checksumB!=packet.checksum)
        return;
    receiveBuffer[packet.seqnum]=packet;
    if(packet.seqnum>=lastAcked)
        lastAcked=packet.seqnum;
    if(packet.seqnum==expectedSeqNum) {
       for(int i=expectedSeqNum;i<=lastAcked;i++) {
           if(receiveBuffer[i].checksum==0)
               break;
           tolayer5(1, receiveBuffer[i].payload);
           expectedSeqNum++;
       }

   }

    checksumB=0;
    ackPkt.seqnum=0;
    ackPkt.acknum = packet.seqnum;
    checksumB+=ackPkt.seqnum+ackPkt.acknum;
    ackPkt.checksum=checksumB;
    tolayer3(1, ackPkt);
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
struct pkt packet;
    packet.checksum=0;
    for(int i=0;i<1000;i++){
        receiveBuffer[i]=packet;
    }
}

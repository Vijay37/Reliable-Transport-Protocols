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
float timer=50;
int aSeqNum=0;
int bSeqNum=0;
struct pkt buffer[1000];
struct pkt globalPkt;
struct pkt localPkt;
int buffLen=0;
int pointer=0;
bool rdyToSnd=true;
/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
    strcpy(globalPkt.payload,message.data);
    buffer[buffLen]=globalPkt;
    buffLen++;
    if(rdyToSnd){
        localPkt=buffer[pointer];
        pointer++;
        localPkt.seqnum=aSeqNum;
        tolayer3(0,localPkt);
        starttimer(0,timer);
        rdyToSnd=false;
    }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  if(packet.acknum==aSeqNum){
      stoptimer(0);
      if(aSeqNum==0)
          aSeqNum=1;
      else
          aSeqNum=0;
      rdyToSnd=true;
  }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    tolayer3(0,localPkt);
    starttimer(0,timer);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{

    if(packet.seqnum == bSeqNum){
        tolayer5(1,packet.payload);
        packet.acknum=bSeqNum;

        if(bSeqNum==0)
            bSeqNum=1;
        else
            bSeqNum=0;
        tolayer3(1,packet);
    }
    else{
        packet.acknum=packet.seqnum;
        tolayer3(1,packet);
    }

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}

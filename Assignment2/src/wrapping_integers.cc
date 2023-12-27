#include "wrapping_integers.hh"

#include <iostream>
#include <math.h>

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { return isn + uint32_t(n); }

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
  uint64_t diff=0;
  uint32_t nr=n.raw_value();
  uint32_t isnr=isn.raw_value();
  if(nr>=isnr){
    diff=nr-isnr;
  }
  else{
    diff=pow(2,32)-1-isnr;
    diff=diff+nr+1;
  }
  
  if(checkpoint<=diff){
    return diff;
  }
  
  /*
  uint64_t num=pow(2, 63)-1;
  num=num+pow(2, 63);
  num=num-pow(2, 32)+1;
  
  uint64_t num = 0xffffffff00000000;
  num=(num & checkpoint);
  num=num+diff;
  if(checkpoint-num+pow(2, 32)<num-checkpoint){
    return num-pow(2, 32);
  }
  */
  uint64_t num=checkpoint-diff;
  num=num & 0x00000000ffffffff;
  if(num>pow(2, 32)-num){
    return checkpoint-num+0x100000000;
  }
  return checkpoint-num;  
}

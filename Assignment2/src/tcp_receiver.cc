#include "tcp_receiver.hh"

#include <algorithm>


using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();
    // ...
    if(head.fin){
      _finReceived=true;
    }
    if(head.syn){
      _synReceived=true;
      _isn=head.seqno;
    }
    // --- Hint ------
    // convert the seqno into absolute seqno
    uint64_t checkpoint = _reassembler.ack_index();
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno > 0 ? abs_seqno - _synReceived : 0;
    // --- Hint ------  

    // ...
    std::string data=seg.payload().copy();
    //cout << data << endl;
    if (!head.syn && abs_seqno == 0 && !data.empty()) data = data.substr(1); 
    _reassembler.push_substring(data, stream_idx, head.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
  if(!_synReceived){
    return {};
  }
  uint64_t abs_seqno=_reassembler.ack_index()+1+_finReceived;
  return wrap(abs_seqno, _isn);
}

size_t TCPReceiver::window_size() const { return _capacity-_reassembler._output.buffer_size(); }

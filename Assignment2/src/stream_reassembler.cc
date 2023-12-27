#include "stream_reassembler.hh"

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    :_output(capacity)
{
  _capacity=capacity;
  buffer_capa=0;
  //cout << "Capacity: " << _capacity << endl;
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
  //cout << "  myobj.push_substring(\"" << data.length() << "\", " << index << ", " << eof << ");" << endl;

  if(eof && _eof<0){
    _eof=index+data.length();
  }
  
  if(_output.buffer_size()==_capacity){
    return;
  }
  
  int ind=0;
  if(ack_index()>index){
    ind=ack_index()-index;
  }
  
  if(ind>=data.length()){
    if(eof){
      _output.end_input();
      _eof=ack_index();
    }
    return;
  }
  int data_allowed=data.length();
  /*
  if((_capacity-buffer_capa-_output.buffer_size())<data_allowed){
    data_allowed=_capacity-buffer_capa-_output.buffer_size();
  }
  */
  bool add_to_vector=false;
  int substr_pos=0;
  std::vector<std::pair<int, int>> new_data;

  for(auto& it : buffer){
    if(ind>=(data_allowed)){
      break;
    }
#if 0
    if(add_to_vector){
      int use_len=it.first-substr_pos<index+data_allowed-substr_pos?it.first-substr_pos:index+data_allowed-substr_pos;
      new_data.emplace_back(substr_pos, use_len);
      add_to_vector=false;
      if(use_len==index+data_allowed-substr_pos){
        break;
      }
      ind=it.first+it.second.length()-index;
    }
#endif
    if((index+ind)<it.first){
      if(index+data_allowed-1<it.first){
        new_data.emplace_back(index+ind, data_allowed);
        ind=data_allowed;
        break;
      }
      else if(index+data_allowed<=it.first+it.second.length()){
        new_data.emplace_back(index+ind, it.first-index-ind);
        ind=data_allowed;
        //ind=ind+it.first-ind;
        break;
      }
      else{
        new_data.emplace_back(index+ind, it.first-index-ind);
        //add_to_vector=true;
        //substr_pos=it.first+it.second.length();
        ind=it.first+it.second.length()-index;
      }
    }
    else if(index+ind<=it.first+it.second.length()-1){
      if(index+data_allowed<=it.first+it.second.length()){
        ind=data_allowed;
        break;
      }
      else{
        //add_to_vector=true;
        //substr_pos=it.first+it.second.length();
        ind=it.first+it.second.length()-index;
      }
    }
    else{
      //add_to_vector=true;
      //substr_pos=index+ind;
    }
  }

#if 0
  if(add_to_vector){
        new_data.emplace_back(substr_pos, index+data_allowed-substr_pos);
        add_to_vector=false;
        //ind=it.first;
  }
#endif
  if(ind < data_allowed) {
    new_data.emplace_back(index+ind, data_allowed-ind);
  }
#if 0
  if(buffer_capa==0){
    new_data.emplace_back(index+ind, data_allowed);
  }
#endif

  for(auto p : new_data){
    int remaining=_capacity-_output.buffer_size();
    if(remaining==0){
      break;
    }
    int use_len=p.second<remaining?p.second:remaining;
    //use_len=p.second;
    if(_eof>0 && p.first+use_len>_eof){
      use_len=_eof-p.first;
    }
    buffer[p.first]=data.substr(p.first-index, use_len);
    buffer_capa+=use_len;
  }
  
  std::vector<int> buffer_remove;
  
  for(auto& it : buffer){
    if(it.first==ack_index()){
        _output.write((it.second));
        buffer_remove.push_back(it.first);
        buffer_capa-=it.second.length();
    }
    if(_eof==ack_index()){
      _output.end_input();
      break;
    }
  }

  for(auto it : buffer_remove){
    buffer.erase(it);
  }
}

size_t StreamReassembler::unassembled_bytes() const { return buffer_capa; }

bool StreamReassembler::empty() const { return buffer_capa==0; }

size_t StreamReassembler::ack_index() const { return _output.bytes_written(); }
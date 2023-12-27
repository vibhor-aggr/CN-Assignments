#include "byte_stream.hh"

#include <algorithm>

// You will need to add private members to the class declaration in `byte_stream.hh`

/* Replace all the dummy definitions inside the methods in this file. */


using namespace std;

ByteStream::ByteStream(const size_t capa) {
  _capacity=capa;
  _buffer.reserve(_capacity);
  _end_write=false;
  _bytes_read=0;
  _bytes_written=0;
}

size_t ByteStream::write(const string &data) {
  size_t remaining_cap=remaining_capacity();
  if(data.length()>remaining_cap) {
    _buffer.append(data, 0, remaining_cap);
    _bytes_written+=remaining_cap;
    return remaining_cap;
  }
  _buffer.append(data);
  _bytes_written+=data.length();
  return data.length();
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
  std::string peek;
  if(len>buffer_size()) {
    peek=_buffer.substr(0, buffer_size());
    return peek;
  }
  peek=_buffer.substr(0, len);
  return peek;
//  return {};
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
  if(len>buffer_size()) {
    set_error();
  }
  _buffer.erase(0, len);
  _bytes_read+=len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
  if(len>buffer_size()) {
    set_error();
    return std::string("");
  }
  std::string peek=_buffer.substr(0, len);
  _buffer.erase(0, len);
  _bytes_read+=len;
  return peek;
//  return {};
}

void ByteStream::end_input() { _end_write=true; }

bool ByteStream::input_ended() const { return _end_write;}

size_t ByteStream::buffer_size() const { return _buffer.length(); }

bool ByteStream::buffer_empty() const { return buffer_size()==0; }

bool ByteStream::eof() const { return buffer_empty() && _end_write; }

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read;  }

size_t ByteStream::remaining_capacity() const { return  _capacity-_buffer.length(); }

#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity)
    : ringBuf_(capacity + 1)
    , readIndex_(0)
    , writeIndex_(0)
    , hasRead_(0)
    , hasWritten_(0)
    , inputEnd_(false)
    , bufError_(false) {}

size_t ByteStream::write(const string &data) {
    size_t len = data.length() > remaining_capacity() ? remaining_capacity() : data.length();
    if (writeIndex_ >= readIndex_) {
        size_t topRes = ringBuf_.size() - writeIndex_;
        if (len <= topRes) {
            std::copy(data.begin(), data.begin() + len, ringBuf_.begin() + writeIndex_);
        } else {
            std::copy(data.begin(), data.begin() + topRes, ringBuf_.begin() + writeIndex_);
            std::copy(data.begin() + topRes, data.begin() + len, ringBuf_.begin());
        }
    } else {
        std::copy(data.begin(), data.begin() + len, ringBuf_.begin() + writeIndex_);
    }
    hasWritten_ += len;
    writeIndex_ = (writeIndex_ + len) % ringBuf_.size();
    return len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t length = len > buffer_size() ? buffer_size() : len;
    std::string data;
    if (writeIndex_ > readIndex_) {
        data.assign(ringBuf_.begin() + readIndex_, ringBuf_.begin() + readIndex_ + len);
    } else {
        size_t topRes = ringBuf_.size() - readIndex_;
        if (length <= topRes) {
            data.assign(ringBuf_.begin() + readIndex_, ringBuf_.begin() + readIndex_ + length);
        } else {
            data.assign(ringBuf_.begin() + readIndex_, ringBuf_.end());
            data.append(std::string(ringBuf_.begin(), ringBuf_.begin() + length - topRes));
        }
    }
    return data;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t length = len > buffer_size() ? buffer_size() : len;
    readIndex_ = (readIndex_ + length) % ringBuf_.size();
    hasRead_ += length;
}

void ByteStream::end_input() { inputEnd_ = true; }

bool ByteStream::input_ended() const { return inputEnd_; }

size_t ByteStream::buffer_size() const { return (writeIndex_ - readIndex_ + ringBuf_.size()) % ringBuf_.size(); }

bool ByteStream::buffer_empty() const { return readIndex_ == writeIndex_; }

bool ByteStream::eof() const { return buffer_empty() && input_ended(); }

size_t ByteStream::bytes_written() const { return hasWritten_; }

size_t ByteStream::bytes_read() const { return hasRead_; }

size_t ByteStream::remaining_capacity() const { return ringBuf_.size() - 1 - buffer_size(); }

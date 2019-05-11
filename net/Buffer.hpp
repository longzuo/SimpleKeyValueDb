#ifndef NET_BUFFER_HPP
#define NET_BUFFER_HPP
#include "string"
#include "unix_header.h"
namespace Net {

class Buffer {
   private:
    std::string buffer;
    int readstart;

   public:
    Buffer(int);
    Buffer(const Buffer&);
    Buffer(Buffer&&);
    ~Buffer() = default;
    Buffer& operator<<(char c);
    Buffer& operator<<(const char*);
    std::string&& readBufferAsString();
    std::string copyBufferAsString(size_t);
    size_t capcity();
    size_t size();
    size_t readFromFd(int);
    size_t writeToFd(int);
    size_t push(const char*, size_t);
    size_t push(const std::string&);
    size_t push(std::string&&);
    void clear();
};

Buffer::Buffer(int size = 256) {
    buffer.reserve(size);
    readstart = 0;
}

Buffer& Buffer::operator<<(char c) {
    // push back
    buffer.push_back(c);
    return *this;
}

Buffer& Buffer::operator<<(const char* s) {
    size_t i = 0;
    buffer.append(s);
    return *this;
}

void Buffer::clear() {
    buffer.clear();
    readstart = 0;
}

std::string&& Buffer::readBufferAsString() {
    // return
    if (size() == 0) {
        return std::move(std::string(""));
    }
    if (readstart == 0) {
        return std::move(buffer);
    } else {
        std::string res(buffer, readstart);
        clear();
        return std::move(res);
    }
}

std::string Buffer::copyBufferAsString(size_t len) {
    if (size() > 0) {
        if (len > size()) {
            len = size();
        }
        return std::string(buffer, readstart, len);
    } else {
        return std::string();
    }
}

size_t Buffer::capcity() {
    //
    return buffer.capacity();
}

size_t Buffer::size() {
    //
    return buffer.size() - readstart;
}

size_t Buffer::push(const char* buf, size_t siz) {
    if (siz <= 0) {
        return 0;
    }
    buffer.reserve(buffer.size() + siz * 2);
    buffer.append(buf, siz);
    return siz;
}

size_t Buffer::push(const std::string& msg) {
    // add
    buffer += msg;
}

size_t Buffer::push(std::string&& msg) {
    if (size() == 0) {
        buffer = std::move(msg);
    } else {
        buffer.append(msg);
    }
    return msg.size();
}

size_t Buffer::readFromFd(int fd) {
    if (fd == -1) {
        return 0;
    }
    // int flags, preflags;
    // flags = preflags = fcntl(fd, F_GETFL, 0);
    // flags |= O_NONBLOCK;
    // flags |= O_NDELAY;
    // fcntl(fd, F_SETFL, flags);
    char buf[512];
    size_t bytes = 0;
    bytes = ::read(fd, buf, 512);
    if (bytes > 0) {
        buffer.append(buf, bytes);
    }

    // reset flags
    // fcntl(fd, F_SETFL, preflags);
    return bytes;
}

size_t Buffer::writeToFd(int fd) {
    // size_t total = size();
    if (size() == 0) {
        return 0;
    }
    size_t wcount = 0;
    // int flags, preflags;
    // flags = preflags = fcntl(fd, F_GETFL, 0);
    // flags |= O_NONBLOCK;
    // flags |= O_NDELAY;
    // fcntl(fd, F_SETFL, flags);
    char* start = const_cast<char*>(buffer.c_str());
    if ((wcount = ::write(fd, start, size())) > 0) {
        readstart += wcount;
    }
    //如果写完了就清空buffer
    if (size() == 0) {
        clear();
    }
    // fcntl(fd, F_SETFL, preflags);
    return wcount;
}

}  // namespace Net

#endif
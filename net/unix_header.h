#ifndef UNIX_NET_LIB_HEADER_H
#define UNIX_NET_LIB_HEADER_H
extern "C" {
#include <arpa/inet.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <linux/tcp.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
}
#endif
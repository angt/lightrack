#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

#include "argz/argz.h"

#ifndef TCP_FASTOPEN
#define TCP_FASTOPEN 23
#endif

struct {
    struct sockaddr_storage bind, to;
    unsigned short bind_port;
    unsigned short to_port;
    int count;
    int timeout;
} lt = {
    .bind_port = 8080,
    .to_port = 8080,
    .count = 3,
    .timeout = 5,
};

static int
lt_sso(int fd, int level, int option, int val)
{
    return setsockopt(fd, level, option, &val, sizeof(val));
}

static int
lt_keepalive(int fd)
{
    if (lt_sso(fd, SOL_SOCKET, SO_KEEPALIVE, 1))
        return 1;

    if (lt_sso(fd, IPPROTO_TCP, TCP_USER_TIMEOUT, 5)) // TODO
        return 1;

    if (lt.count > 0 &&
        lt_sso(fd, IPPROTO_TCP, TCP_KEEPCNT, lt.count))
        return 1;

    if (lt.timeout > 0 &&
        (lt_sso(fd, IPPROTO_TCP, TCP_KEEPIDLE, lt.timeout) ||
         lt_sso(fd, IPPROTO_TCP, TCP_KEEPINTVL, lt.timeout)))
        return 1;

    return 0;
}

static int
lt_fastopen(int fd)
{
    if (lt_sso(fd, IPPROTO_TCP, TCP_FASTOPEN, 1))
        return 1;

    return 0;
}

static void
lt_setup_port(struct sockaddr_storage *ss, uint16_t port)
{
    switch (ss->ss_family) {
    case AF_INET:
        ((struct sockaddr_in *)ss)->sin_port = htons(port);
        break;
    case AF_INET6:
        ((struct sockaddr_in6 *)ss)->sin6_port = htons(port);
        break;
    }
}

static int
lt_init(int argc, char **argv)
{
    unsigned count = lt.count;
    unsigned timeout = 1000U * lt.timeout;

    struct argz bindz[] = {
        {NULL, "IPADDR", &lt.bind, argz_addr},
        {NULL, "PORT", &lt.bind_port, argz_ushort},
        {NULL},
    };

    struct argz toz[] = {
        {NULL, "IPADDR", &lt.to, argz_addr},
        {NULL, "PORT", &lt.to_port, argz_ushort},
        {NULL},
    };

    struct argz ltz[] = {
        {"bind", NULL, &bindz, argz_option},
        {"to", NULL, &toz, argz_option},
        {"timeout", "SECONDS", &timeout, argz_time},
        {"count", "COUNT", &count, argz_ulong},
        {NULL},
    };

    if (argz(ltz, argc, argv))
        return 1;

    if (!lt.to.ss_family) {
        fprintf(stderr, "option `host' is mandatory\n");
        return 1;
    }

    if (count > INT_MAX) {
        fprintf(stderr, "option `count' is too big\n");
        return 1;
    }

    timeout /= 1000U;

    if (timeout > INT_MAX) {
        fprintf(stderr, "option `timeout' is too big\n");
        return 1;
    }

    lt.count = (int)count;
    lt.timeout = (int)timeout;

    // or use v4mapped ?
    if (lt.to.ss_family != lt.bind.ss_family) {
        fprintf(stderr, "host and bind are not compatible\n");
        return 1;
    }

    lt_setup_port(&lt.bind, lt.bind_port);
    lt_setup_port(&lt.to, lt.to_port);

    return 0;
}

static int
lt_addrlen(struct sockaddr_storage *ss)
{
    return (ss->ss_family == AF_INET) ? sizeof(struct sockaddr_in)
                                      : sizeof(struct sockaddr_in6);
}

static int
lt_bind(int fd)
{
    int ret = bind(fd, (struct sockaddr *)&lt.bind, lt_addrlen(&lt.bind));

    if (ret == -1) {
        perror("bind");
        return 1;
    }

    return 0;
}

static int
lt_connect(int fd)
{
    int ret = connect(fd, (struct sockaddr *)&lt.to, lt_addrlen(&lt.to));

    if (ret == -1) {
        perror("connect");
        return 1;
    }

    return 0;
}

static int
lt_wait(int fd)
{
    while (1) {
        char tmp;
        int ret = read(fd, &tmp, 1);

        if (ret == -1) {
            perror("read");
            return 1;
        }

        if (!r)
            break;
    }

    return 0;
}

static int
lt_socket(void)
{
    int fd = socket(lt.bind.ss_family, SOCK_STREAM, IPPROTO_TCP);

    if (fd == -1) {
        perror("socket");
        return -1;
    }

    if (lt_keepalive(fd))
        fprintf(stderr, "couldn't setup keepalive\n");

    if (lt_fastopen(fd))
        fprintf(stderr, "couldn't setup fastopen\n");

    return fd;
}

int
main(int argc, char **argv)
{
    if (lt_init(argc, argv))
        return 1;

    while (1) {
        int fd = lt_socket();

        if (fd < 0)
            return 1;

        if (!lt_bind(fd)) {
            while (lt_connect(fd))
                sleep(lt.timeout);
        }

        lt_wait(fd);

        if (fd >= 0)
            close(fd);

        sleep(lt.timeout);
    }

    return 0;
}

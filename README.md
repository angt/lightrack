# lightrack

WIP

```
socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) = 3                             │socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) = 3
setsockopt(3, SOL_SOCKET, SO_KEEPALIVE, [1], 4) = 0                       │setsockopt(3, SOL_SOCKET, SO_KEEPALIVE, [1], 4) = 0
setsockopt(3, SOL_TCP, TCP_USER_TIMEOUT, [5], 4) = 0                      │setsockopt(3, SOL_TCP, TCP_USER_TIMEOUT, [5], 4) = 0
setsockopt(3, SOL_TCP, TCP_KEEPCNT, [3], 4) = 0                           │setsockopt(3, SOL_TCP, TCP_KEEPCNT, [3], 4) = 0
setsockopt(3, SOL_TCP, TCP_KEEPIDLE, [1], 4) = 0                          │setsockopt(3, SOL_TCP, TCP_KEEPIDLE, [1], 4) = 0
setsockopt(3, SOL_TCP, TCP_KEEPINTVL, [1], 4) = 0                         │setsockopt(3, SOL_TCP, TCP_KEEPINTVL, [1], 4) = 0
setsockopt(3, SOL_TCP, TCP_FASTOPEN, [1], 4) = 0                          │setsockopt(3, SOL_TCP, TCP_FASTOPEN, [1], 4) = 0
bind(3, {sa_family=AF_INET, sin_port=htons(8080), sin_addr=A}, 16) = 0    │bind(3, {sa_family=AF_INET, sin_port=htons(8080), sin_addr=B}, 16) = 0
connect(3, {sa_family=AF_INET, sin_port=htons(8080), sin_addr=B}, 16) = 0 │connect(3, {sa_family=AF_INET, sin_port=htons(8080), sin_addr=A}, 16) = 0
read(3,                                                                   │read(3,
```

rm app-server.out
gcc app-server.c -I ../network-layer/server/include -L ../network-layer/server/lib -lssnl -o app-server.out
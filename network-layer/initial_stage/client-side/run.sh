rm *.out
gcc app-client.c -I ../network-layer/client/include -L ../network-layer/client/lib -lcsnl -o app-client.out
gcc shutdown-server.c -I ../network-layer/client/include -L ../network-layer/client/lib -lcsnl -o shutdown-server.out
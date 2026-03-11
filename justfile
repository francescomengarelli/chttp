client:
    set -e
    @make examples DEBUG=1 >/dev/null
    @clear
    @./build/bin/debug/ex_client

server:
    set -e
    @make examples DEBUG=1 >/dev/null
    @clear
    @./build/bin/debug/ex_server

test:
    make test DEBUG=1

clean:
    @make clean

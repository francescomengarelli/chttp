# chttp

chttp is a lightweight, pure C99 HTTP/1.1 library with no external
dependencies. It provides both a client and server API under a single
interface.

## Supported Platforms

Only POSIX compliant OS are supported. Tested only on Arch Linux currently.

## Dependencies

The library does not require any external dependency.

## Building, Testing and Examples

You can **build** the library using `make`. you can build only the server library by running
`make server`, and only the client library by running `make client`. if you need both,
you can simply run `make`.

To **test** the library, you can run `make test`.

To **build the examples**, you can run `make examples`. You'll find the compiled examples in
`build/bin/release/` by default.

### Flags

- You can set the `DEBUG` flag to `1` if you want to build in debug mode. default value of `DEBUG` is
  `0`, i.e. release mode.

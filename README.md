# PrintTarantool
In this repository you can find utility for printing [spaces](https://www.tarantool.io/en/doc/latest/book/box/data_model/#spaces) from [tarantool_web](https://www.tarantool.io/en/developers/ "Tarantool") databases. This application using Tarantool's c-connector to connect to Tarantool.

## Dependencies
    * [cconn](https://github.com/tarantool/tarantool-c.git "C-Connector")
    * [tarantool](https://github.com/tarantool/tarantool.git "Tarantool")

## Build

To build this program from sources one should have cmake/make utilities. Currently supported
OS is Linux.

Steps to build:
```
mkdir build
cd build
cmake ..
make
```

## Testing

```
make test
```
You can view the test log file via this path ./build/Testing/Temporary/LastTest.log.


## Install

Steps to install:
```
make install
```
After this you can execute command `print_space` *space_id* *IP:host* in any directory. For example, `print_space 512 127.0.0.1:3301`

# HTTP server in C++

In order to understand more about HTTP and it's implemetations, I will try to write my own HTTP server in C++ with (almost) all MUST and some SHOULD features described in [RFC 2616](https://datatracker.ietf.org/doc/html/rfc2616).

### Intended functionalities: 

- Basic request methods such as HEAD, GET, POST, OPTIONS
- Headers
- Routing
- Persistent connections

### Namespaces 

server
|----basic
|----network
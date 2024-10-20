
# gRPCurl Installation and Usage Guide

**gRPCurl** is a command-line tool that lets you interact with gRPC servers by querying them with requests, similar to how `curl` works for HTTP/REST APIs.

## Installation of gRPCurl

You can install gRPCurl by downloading the latest release for your platform. Below is an example for installing it on Linux:

```bash
wget https://github.com/fullstorydev/grpcurl/releases/download/v1.9.1/grpcurl_1.9.1_linux_amd64.deb
sudo dpkg -i grpcurl_1.9.1_linux_amd64.deb
```

## Checking Installed Version

To verify that gRPCurl is correctly installed, you can check the version:

```bash
$ grpcurl --version
grpcurl v1.9.1
```

## Getting Help

For a complete list of options and commands, use the `--help` flag:

```bash
$ grpcurl --help
Usage:
  grpcurl [flags] [address] [list|describe] [symbol]
...
```

## Default Behavior: TLS Enabled

By default, gRPCurl assumes that gRPC services are running with TLS enabled. If the server is not using TLS, you'll receive an error like this:

```bash
$ grpcurl localhost:6000 list
Failed to dial target host "localhost:6000": tls: first record does not look like a TLS handshake
```

### Solution: Use `--plaintext` for Non-TLS Connections

If the gRPC service is not using TLS, you need to add the `--plaintext` flag:

```bash
$ grpcurl --plaintext localhost:6000 list
fibonacci.FibonacciSlowService
grpc.reflection.v1.ServerReflection
grpc.reflection.v1alpha.ServerReflection
```

## Listing Services from the `.proto` File

To list the services defined in a `.proto` file, use:

```bash
$ grpcurl -proto fibonacci.proto list
fibonacci.FibonacciService
fibonacci.FibonacciSlowService
```

## Listing Service Methods

To list all the available RPC methods of a specific service:

```bash
$ grpcurl --plaintext localhost:6000 list fibonacci.FibonacciSlowService
fibonacci.FibonacciSlowService.GetFibonacciList
```

## Describing a Specific RPC

To get detailed information about an RPC method:

```bash
$ grpcurl --plaintext localhost:6000 describe fibonacci.FibonacciSlowService.GetFibonacciList
fibonacci.FibonacciSlowService.GetFibonacciList is a method:
rpc GetFibonacciList ( .fibonacci.FibonacciRequest ) returns ( .fibonacci.FibonacciListResponse );
```

## Describing an RPC Message Type

To describe the message format required for a particular RPC:

```bash
$ grpcurl --plaintext localhost:6000 describe .fibonacci.FibonacciRequest
fibonacci.FibonacciRequest is a message:
message FibonacciRequest {
  uint64 number = 1;
}
```

### Generating a Message Template

You can also request a message template that gives a structure for JSON input:

```bash
$ grpcurl --plaintext -msg-template localhost:6000 describe .fibonacci.FibonacciRequest
fibonacci.FibonacciRequest is a message:
message FibonacciRequest {
  uint64 number = 1;
}

Message template:
{
  "number": "0"
}
```

## Calling an RPC Method: `GetFibonacciList`

Here’s how to invoke an RPC method and receive the Fibonacci list:

```bash
$ grpcurl --plaintext -emit-defaults -d '{ "number": "8" }' localhost:6000 fibonacci.FibonacciSlowService/GetFibonacciList
{
  "number": [
    "0",
    "1",
    "1",
    "2",
    "3",
    "5",
    "8",
    "13"
  ]
}
```

## Calling an RPC Method with Streaming: `GetFibonaccisStream`

If the service uses streaming, gRPCurl can handle it as well. Here's how you invoke a streaming RPC:

```bash
$ grpcurl --plaintext -emit-defaults -d '{ "number": "3" }' localhost:6000 fibonacci.FibonacciService/GetFibonaccisStream
{
  "number": "0"
}
{
  "number": "1"
}
{
  "number": "1"
}
```

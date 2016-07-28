## Demos
Various examples showcasing the different approaches to concurrency.

## Examples
I have made various example showcasing deferents forms of socket I/O handling, blocking, multithread and concurrent,
, to run the examples you only need Linux and [Clang]. In root folder you can find compile.sh to compile the sources.



- to run the demos:

```sh
 ./single_thread_blocking/demo.o
 ./multi_thread_blocking/worker_demo.o
 ./event_driven/evented_demo.o
```

- once they are running, you can run this scripts to see how it works:

```sh
 ./test/simple_curl.sh
 ./test/concurrent_curl.sh
```

[Clang]:'http://clang.llvm.org/'

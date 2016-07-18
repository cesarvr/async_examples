rm multi_thread_blocking/worker_demo.o
rm single_thread_blocking/demo.o
rm event_driven/evented_demo.o



#single
clang++ -x c++ -std=c++11 -lpthread single_thread_blocking/demo.cpp   -o single_thread_blocking/demo.o

#multithread
clang++ -x c++ -std=c++11 -lpthread multi_thread_blocking/demo.cpp   -o multi_thread_blocking/worker_demo.o

#evented
clang++ -x c++ -std=c++11 -lpthread event_driven/demo.cpp -o event_driven/evented_demo.o

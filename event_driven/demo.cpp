#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <sys/epoll.h>

#include <vector>
#include <iostream>
#include <sys/epoll.h> //Event driven I/O.
#include <fcntl.h> // manipulate file descriptor

#define MAX_EVENTS     5

template <typename T>
void checkError(T&& t, std::string message){

  if (t == -1) {
     perror( message.c_str() );
     exit(EXIT_FAILURE);
  }

}

template <typename T>
void ShowEvent(T* events, int amountOfEvents){

  for(int x=0; x<amountOfEvents; x++ ){


    if(events[x].events & EPOLLOUT)
      std::cout << "epoll -> EPOLLOUT (ready for write)";

    if(events[x].events & EPOLLERR)
      std::cout << "epoll -> EPOLLERR ( error !! )";

    if(events[x].events & EPOLLHUP)
      std::cout << "epoll -> EPOLLHUP ( A hangup has occurred !! )";

    if(events[x].events & EPOLLIN)
      std::cout << "epoll -> EPOLLIN (ready for read)";



    std::cout << "   epoll_event.epoll_data.fd -> " << events[x].data.fd << std::endl;
  }
}



void SetNonBlocking(int file_descriptor){
  int flags = fcntl(file_descriptor, F_GETFL);

  if(!(flags & O_NONBLOCK)) {

    puts("Non-Block flag Yes");
    flags |= O_NONBLOCK;  /* Enable O_NONBLOCK bit */
    fcntl(file_descriptor, F_SETFL, flags);

  }else
    puts("Non-Block flag No");
}

const std::string resp = "HTTP/1.0 200 OK \n Date: Mon, 11 July 2016 23:59:59 GMT\n Content-Type: text/html  \n\n <html> <head> <title> Hi!! </title>  </head> <body> <h1>Hello World</h1> </body>  </html> \n";


void Write(int file_descriptor){
  write(file_descriptor, resp.c_str(), resp.size());
}

void Read(int file_descriptor) {
  const unsigned int MAX_BUF_LENGTH = 1024;
  char buffer[MAX_BUF_LENGTH];

  checkError( read( file_descriptor, buffer, MAX_BUF_LENGTH ) , "can't read from this fd_socket.");

  printf("\n \n reading data \n ============== \n %s \n ============== \n ", buffer);
}


template <typename T>
void ReadAsync(T* events, int amountOfEvents){



  for(int x=0; x<amountOfEvents; x++ ){
    if(events[x].events & EPOLLIN) {

      int file_descriptor = events[x].data.fd;
      int read_size;

      Read(file_descriptor);
      Write(file_descriptor);

    }
  }

}



class EventIO {

private:
   int epollfd;

public:

   EventIO(): epollfd{0} {
     std::cout << "creating epoll ";

     epollfd = epoll_create(5);

     if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
     }

     std::cout << "  ...created " << std::endl;
   }

   void Unsuscribe(int fd){
      //EPOLL_CTL_DEL

      struct epoll_event ep_event;
      ep_event.events = EPOLLIN & EPOLLOUT;
      ep_event.data.fd = fd;

      checkError(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ep_event), "epoll_ctl: unsuscribe error!");

      std::cout << "stop listening -> " << fd << std::endl;
   }

   void Subscribe(int fd) {
     std::cout << "listen for I/O events: " << fd << std::endl;

      struct epoll_event listen_in;
      listen_in.events = EPOLLIN | EPOLLOUT;
      listen_in.data.fd = fd;

      checkError( epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &listen_in) ,
                  "epoll_ctl: listen_sock" );


      std::cout << "testing event listening" << std::endl;

      struct epoll_event events[MAX_EVENTS];

      std::cout << "epoll wait..." << std::endl;
      int amountOfEvents = 0;

      checkError( ( amountOfEvents = epoll_wait(epollfd, events, MAX_EVENTS, -1) ),
                  "134: error in epoll_wait" );

      ShowEvent<epoll_event>(events, amountOfEvents);

      std::cout << "Read/Write Socket" << std::endl;
      ReadAsync<epoll_event>(events, amountOfEvents);
   }



};



int init_socket() {

  //fd means file descriptor.
  int socket_fd , client_sock , c;
  struct sockaddr_in server , client;

  //Create socket
  socket_fd = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_fd == -1)
  {
    printf("Could not create socket");
  }

  puts("Socket created");

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( 8888 );


  //Bind
  checkError( bind(socket_fd,(struct sockaddr *)&server , sizeof(server)),
              "Error binding socket. " );

  //Listen
  listen(socket_fd , 3);

  int len = sizeof(struct sockaddr_in);

  //Evented Socket hook.
  puts("Hooking evented_io");

  EventIO eventIO;

  puts("Listening in http://localhost:8888.");


  puts("Setting socket int Non-Blocking mode.");


  while( (client_sock = accept(socket_fd, (struct sockaddr *)&client, (socklen_t *) &len )) )
  {
    puts("connection accepted");

    puts("Setting conection_socket int Non-Blocking mode.");
    SetNonBlocking(client_sock);

    puts("Listening for I/O events");
    eventIO.Subscribe(client_sock);

    std::cout << "bye bye" << std::endl;

    close(client_sock);
  }

  close(client_sock);

  return 0;
}


int main(){
   std::cout << "Event I/O example" << std::endl;
   init_socket();

   return 0;
}

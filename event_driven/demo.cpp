#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <sys/epoll.h>

#include <vector>
#include <iostream>
#include <sys/epoll.h> //Event driven I/O.
#include <fcntl.h> // manipulate file descriptor

#define MAX_EVENTS 10

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


class HTTPServerIO {
private:
  const std::string resp = "HTTP/1.0 200 OK \n Date: Mon, 11 July 2016 23:59:59 GMT\n Content-Type: text/html  \n\n <html> <head> <title> Hi!! </title>  </head> <body> <h1>Hello World</h1> </body>  </html> \n";

public:
   void Write(int file_descriptor){
     write(file_descriptor, resp.c_str(), resp.size());
   }

   void Read(int file_descriptor) {
     const unsigned int MAX_BUF_LENGTH = 1024;
     char buffer[MAX_BUF_LENGTH];

     checkError( read( file_descriptor, buffer, MAX_BUF_LENGTH ) , "can't read from this fd_socket.");

     printf("\n \n reading data \n ==============\n %s \n==============\n", buffer);
   }
};

class Connection {
private:
  int epollfd;
  int socketfd;
  HTTPServerIO serverIO;

public:

  Connection( int epollFileDesc, int socketFileDesc ){

    Subscribe(epollFileDesc, socketFileDesc, EPOLLIN);

    epollfd = epollFileDesc;
    socketfd = socketFileDesc;
  }


  void Subscribe(int epollFileDesc, int socketFileDesc, uint32_t event  ){

    struct epoll_event listen;
    listen.events = EPOLLIN;
    listen.data.fd = socketFileDesc;

    checkError( epoll_ctl(epollFileDesc, EPOLL_CTL_ADD, socketFileDesc, &listen) ,
                "Connection Class: Error listen socketfd" );
  }

  void Unsubscribe(int epollFileDesc, struct epoll_event& listen ){

    checkError( epoll_ctl(epollFileDesc, EPOLL_CTL_DEL, listen.data.fd , &listen) ,
                "Connection Class: Error listen socketfd" );
  }

  int Accept() {

   std::cout << "accepting connection" << std::endl;
   struct sockaddr_in client;
   int len = sizeof(struct sockaddr_in);

   return accept(socketfd, (struct sockaddr *)&client, (socklen_t *) &len  );
  }

  void Listen() {
    struct epoll_event events[MAX_EVENTS];
    int amountOfEvents = 0;

    std::cout << "epoll wait...";

    checkError( ( amountOfEvents = epoll_wait(epollfd, events, MAX_EVENTS, -1) ),
                "134: error in epoll_wait" );

    ShowEvent(events, amountOfEvents);

    for(int i=0; i<amountOfEvents; i++) {

        if( events[i].events & EPOLLIN && events[i].data.fd == socketfd){
          int client_socket_fd = Accept();

          Subscribe(epollfd, client_socket_fd, EPOLLIN | EPOLLOUT);
          std::cout << "accepted" << std::endl;
        }

        if(events[i].events & EPOLLIN && events[i].data.fd != socketfd) {
          auto socketFD = events[i].data.fd;

          SetNonBlocking(socketFD);
          serverIO.Read(socketFD);
          //serverIO.Write(socketFD);
          std::cout << "Unsubscribe" << std::endl;
          Unsubscribe(epollfd, events[i]);
          close(socketFD);
        }
    }

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

  //Evented Socket hook.
  puts("Hooking evented_io");

  int epollFileDesc;

  checkError( ( epollFileDesc = epoll_create(20) ), "error epoll_create");



  puts("Listening in http://localhost:8888.");

  //Listen
  listen(socket_fd , 3);

  puts("Setting socket int Non-Blocking mode.");

  SetNonBlocking(socket_fd);

  Connection connection(epollFileDesc, socket_fd);

  while( true )
  {
    connection.Listen();
  }


  return 0;
}


int main(){
   std::cout << "Event I/O example" << std::endl;
   init_socket();

   return 0;
}

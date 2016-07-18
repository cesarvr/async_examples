
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <sys/epoll.h>

#include <vector>
#include <iostream>
#include <sys/epoll.h> //Event driven I/O. 


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

   void Listen(int fd) {
     std::cout << "listen for I/O events: ";

      struct epoll_event ep_event; 
      ep_event.events = EPOLLIN;
      ep_event.data.fd = fd;

      if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ep_event) == -1) {
         perror("epoll_ctl: listen_sock");
         exit(EXIT_FAILURE);
      }  

      std::cout << "  ...cool " << std::endl;
   }

};



void handle_client(int file_descriptor){
  const unsigned int MAX_BUF_LENGTH = 1024;

  std::vector<char> buffer(MAX_BUF_LENGTH); 
  std::string data; 

  std::string resp = "HTTP/1.0 200 OK \n Date: Mon, 11 July 2016 23:59:59 GMT\n Content-Type: text/html  \n\n <html> <head> <title> Hi!! </title>  </head> <body> <h1>Hello World</h1> </body>  </html> \n";

  int read_size = 0; 
  std::cout << "waiting for data.." << std::endl;
  
 
  while( ( read_size = read( file_descriptor, buffer.data(), MAX_BUF_LENGTH ) ) > 0 ) {
    printf(" worker -> %d \n ", read_size);


    auto rsize = resp.size() * sizeof(char);

    std::cout << "sending resp data_size: " << rsize << std::endl;
    write(file_descriptor, resp.c_str(), rsize); 


    data.append( buffer.begin(), buffer.end() );
   
     
    close(file_descriptor);
  }

  std::cout << "loggin: \n" << data << std::endl;

}


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
  if( bind(socket_fd,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
        //print the error message
     perror("bind failed. Error");
     return 1;
  }


  //Evented Socket hook.
  puts("Hooking evented_io"); 
  EventIO eventIO; 

  eventIO.Listen(socket_fd);

  puts("Listening in http://localhost:8888");
     

  //Listen
  listen(socket_fd , 3);
 
  int len = sizeof(struct sockaddr_in);  

  while( (client_sock = accept(socket_fd, (struct sockaddr *)&client, (socklen_t *) &len )) )
  {
    puts("connection accepted");
     
    // std::thread worker{ handle_client, client_sock };	

     //th_pool.add_worker( std::move(worker) );

     std::cout << "bye bye" << std::endl;
     close(client_sock);
  }

  return 0;
}


int main(){
   std::cout << "Event I/O example" << std::endl;	
   init_socket();

   return 0;
}



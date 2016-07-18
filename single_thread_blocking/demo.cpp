#include <stdio.h>

#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#include <sys/epoll.h>
#include <vector>
#include <iostream>


void handle_client(int file_descriptor){
  const unsigned int MAX_BUF_LENGTH = 1024;

  std::vector<char> buffer(MAX_BUF_LENGTH); 
  std::string data; 

  std::string resp = "HTTP/1.0 200 OK \n Date: Mon, 11 July 2016 23:59:59 GMT\n Content-Type: text/html  \n\n <html> <head> <title> Hi!! </title>  </head> <body> <h1>Hello World</h1> </body>  </html> \n";

  int read_size = 0; 
  std::cout << "waiting for data.." << std::endl;
  
 
  while( ( read_size = read( file_descriptor, buffer.data(), MAX_BUF_LENGTH ) ) > 0 ) {
    printf(" reading -> %d \n ", read_size);


    auto rsize = resp.size() * sizeof(char);

    std::cout << "sending resp data_size: " << rsize << std::endl;
    write(file_descriptor, resp.c_str(), rsize); 


    data.append( buffer.begin(), buffer.end() );
    
    close(file_descriptor);
  }

  std::cout << "loggin: \n" << data << std::endl;

}

int init_socket() {
	
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("Listening in http://localhost:8888");
     
    //Listen
    listen(socket_desc , 3);
  
  int len = sizeof(struct sockaddr_in);  
	
  while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *) &len )) )
  {
     puts("Connection accepted");
     
     handle_client(client_sock);
  }

  return 0;
}


int main(){
   std::cout << "Single Threaded" << std::endl;	
   init_socket();

   return 0;
}



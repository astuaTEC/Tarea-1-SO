FROM ubuntu
COPY src/client ./client   
RUN apt update
RUN apt install -y build-essential
RUN apt-get install -y manpages-dev
EXPOSE 8080  
RUN cd client  
RUN pwd
RUN ls
RUN gcc client/client.c -o client/client.out -lpthread

FROM ubuntu
# WORKDIR /home/saymon/Documents/C Programs/Sistemas Operativos - Tarea 1 //Directorio donde este e cliente.c 
COPY src ./client   
RUN apt update
RUN apt install -y build-essential
RUN apt-get install -y manpages-dev
EXPOSE 9000  
RUN cd client  
RUN pwd
RUN ls
RUN gcc client/client.c -o client/client.out -lpthread

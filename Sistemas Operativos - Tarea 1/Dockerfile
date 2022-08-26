FROM ubuntu
WORKDIR ./src //Directorio donde este e cliente.c 
RUN ls
COPY src ./src   //CArpeta donde esta el. c del cliente 
RUN apt update
RUN apt install -y build-essential
RUN apt-get install -y manpages-dev
EXPOSE 9000  //Puerto dondese está cpounicando el cliente
RUN cd src   //Llamar la carpeta del cliente 
RUN pwd
RUN ls
RUN gcc src/client.c -o src/client.out -lpthread  //Se copia el comando con el que compilas el cliente 

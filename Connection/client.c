#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
  
int main(int argc, char const* argv[])
{

    // image serializer
    FILE *fp;
    char *buffer;
    long file_size;
    size_t result;

    fp = fopen("josu.jpg", "rb");  
    if (fp == NULL) {
        printf("Error: Unable to open file.\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);  // Determine the size of the image file
    file_size = ftell(fp);
    rewind(fp);

    buffer = (char *)malloc(sizeof(char) * file_size);  // Allocate memory for the image data
    if (buffer == NULL) {
        printf("Error: Unable to allocate memory.\n");
        exit(1);
    }

    result = fread(buffer, 1, file_size, fp);  // Read the image data from the file into the allocated memory
    if (result != file_size) {
        printf("Error: Unable to read file.\n");
        exit(1);
    }

    fclose(fp);  // Close the file

    char image_serialized[file_size];  // Convert the image data to a char array
    for (int i = 0; i < file_size; i++) {
        image_serialized[i] = buffer[i];
    }

    free(buffer);  // Free the memory used for the image data

    //
    
    //client initiator

    int status, valread, client_fd;
    struct sockaddr_in serv_addr;

    // for (int i = 0; i < file_size; i++) {
    //      printf("Index %d: %02X\n", i, (unsigned char)image_serialized[i]);
    // }
    
    char buffer2[1024] = { 0 };
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
  
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if ((status
         = connect(client_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    // Open a file for writing
    FILE *file = fopen("output2.txt", "w");

    // Check if the file was opened successfully
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    // Write the buffer to the file
    fwrite(image_serialized, sizeof(char), sizeof(image_serialized), file);

    // Close the file
    fclose(file);

    int flag = file_size/1024;
    send(client_fd, flag, 1, 0);
    int counter = 0;
    while(counter!=flag){
        char dest_array[1024];
        for (int i = 0; i < 1024; i++) {
            dest_array[i] = image_serialized[counter*1024 + i];
        }
        send(client_fd, dest_array, 1024, 0);
        printf("Hello message sent\n");
        valread = read(client_fd, buffer2, 1024);
        printf("%s\n", buffer2);
        counter++;
    }
    char* stop="$";
    send(client_fd, stop, sizeof(stop), 0);
    // closing the connected socket
    close(client_fd);
    return 0;
}
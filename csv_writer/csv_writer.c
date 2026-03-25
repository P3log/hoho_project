#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 128

int writeLabels(char* dest) {
    int fd = open(dest, O_CREAT|O_APPEND|O_RDWR, 0664);
    if (fd == -1){ // Error
        perror("Could not create the database\n");
        return -1;
    }
    char buffer[] = "DATE;TIME;HUMIDITY (\%);TEMPERATURE (°C)\n";
    int status = write(fd, buffer, strlen(buffer)+1);
    if (status == -1) {
        close(fd);
        return -2;
    }
    close(fd);
    
    return 0;
}

int write_to_file(char* dest, char* content, int contentSize){
    int fileExists = access(dest, F_OK);
    if (fileExists == -1) {
        int res = writeLabels(dest);
        if (res == -2) return -2;
    }
    int fd = open(dest, O_APPEND|O_RDWR, 0664);
    if (fd == -1){ // Error
        perror("Could not access the database\n");
        return -1;
    }
    int status = write(fd, content, contentSize);
    if (status == -1){
        perror("An error occured when writing to the database\n");
        close(fd);
        return -2;
    }
    close(fd);
    return 0;
}

int get_date(time_t timeReference, char* buffer, int offset){
    // format : YYYY-MM-DD
    struct tm *t = localtime(&timeReference);
    if (t == NULL){
        perror("localtime error\n");
        return -3;
    }
    strftime(buffer + offset, 11, "%Y-%m-%d", t);
    
    return 0;
}

int get_time(time_t timeReference, char* buffer, int offset){
    // format HH:MM:SS
    struct tm *t = localtime(&timeReference);
    if (t == NULL){
        perror("localtime error\n");
        return -3;
    }
    strftime(buffer + offset, 9, "%H:%M:%S", t);
    return 0;
}

void resetBuffer(char* buffer, int bufferSize){
    for (int i=0; i<bufferSize; i++){
        buffer[i] = '\0';
    }
}

void formatContent(char* buffer, char* date, char* time, char* humidity, char* temperature){
    // YYYY-MM-DD;HH:mm:ss;HUMIDITY;TEMPERATURE\n + '\0'
    resetBuffer(buffer, BUFFER_SIZE);
    snprintf(buffer, BUFFER_SIZE, "%s;%s;%s;%s\n", date, time, humidity, temperature);
}

void write_to_logs(char* message){
    char logfile[] = "../logs/journal.log";
    int fd = open(logfile, O_CREAT|O_APPEND|O_RDWR, 0664);
    if (fd == -1){
        perror("Unfortunately could not trace logs");
    }
    write(fd, message, strlen(message));
    close(fd);
}

void get_message(int errorCode, char* buffer, int offset){
    switch(errorCode){
        case -1:
            strcpy(buffer + offset, "Could not access the database\n");
            break;
        case -2:
            strcpy(buffer + offset, "An error occured when writing to the database\n");
            break;
        case -3:
            strcpy(buffer + offset, "Error while trying to set localtime\n");
            break;
    }
}

void usage(char* program){
    printf("Usage :\n\t %s [HUMIDITY] [TEMPERATURE]\n\n\tHUMIDITY\ttype:float\nTEMPERATURE\ttype: float\n", program);
}


void checkIfError(int errorCode){
    char buffer[128] = {0};
    switch (errorCode){
        case -1:
            get_message(errorCode, buffer, 20);
            write_to_logs(buffer);
            break;
        case -2:
            get_message(errorCode, buffer, 20);
            write_to_logs(buffer);
            break;
        case -3:
            get_message(errorCode, buffer, 20);
            write_to_logs(buffer);
        default:
            // no error : keep going
            break;
    }
}

int main(int argc, char* argv[]){
    if (argc != 3){
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    time_t now = time(NULL);
    char t_date[11];
    char t_time[9];
    char buffer[BUFFER_SIZE];
    
    resetBuffer(t_date, 11);
    resetBuffer(t_time, 9);
    resetBuffer(buffer, BUFFER_SIZE);
    
    int errorCode;
    errorCode = get_date(now, t_date, 0);
    checkIfError(errorCode);

    errorCode = get_time(now, t_time, 0);
    checkIfError(errorCode);
    
    formatContent(buffer, t_date, t_time, argv[1], argv[2]);
    
    // printf("%s\n", buffer);
    
    char destFile[] = "../db/measures.csv";
    errorCode = write_to_file(destFile, buffer, strlen(buffer));

    resetBuffer(buffer, BUFFER_SIZE);
    get_date(now, buffer, 0);
    get_time(now, buffer, 11);
    checkIfError(errorCode);

    return EXIT_SUCCESS;

}

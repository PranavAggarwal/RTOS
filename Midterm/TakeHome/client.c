#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

char sensor_data[5][1024];				// Sensor data

void getdata(int sensor_no, int cli_sock){
	char *sensor_number;
	sprintf(sensor_number, "%d", sensor_no);

	// Sending message to server
	printf("\nAsking server for sensor%d data\n", sensor_no);
	write(cli_sock, sensor_number, sizeof(sensor_number));
	 
    // Receiving message from server   
    strcpy(sensor_data[sensor_no - 1], "");
    printf("Receiving message from server\n");
    read(cli_sock, sensor_data[sensor_no - 1], sizeof(sensor_data[sensor_no - 1]));
    
    // Printing reply from server and time taken by servver to respond back
    printf("Sesnor%d data: %s\n",sensor_no, sensor_data[sensor_no - 1]);
}

int main(int argc, char const *argv[])
{
	int cli_sock;
	struct sockaddr_in serv_add;

	// Creating CSV file
	FILE *csv_file;
	char *filename = "sensor_data.csv";
	csv_file = fopen(filename, "w");
	
	// Adding the header to the file
	fprintf(csv_file, "Timestamp, Sensor1_data, Sensor2_data, Sensor3_data, Sensor4_data, Sensor5_data\n");
	fclose(csv_file);		// Closing file so that we can see the update

	// Creating the socket
	printf("Creating the socket\n");
	if((cli_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket error");
		exit(1);
	}

	// Setting server address
	printf("Setting server address\n");
	memset(&serv_add, 0, sizeof(serv_add));
	serv_add.sin_family = AF_INET;
    serv_add.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_add.sin_port = htons(8000);

    // Sending the connection request to server
	printf("Waiting for connection\n");
    if((connect(cli_sock, (struct sockaddr *)&serv_add, sizeof(serv_add))) < 0){
    	perror("Connection failed");
    	exit(1);
    }
    printf("Connection established\n\n");

    int timestamp = 1;

    // Getting sensor data from sensor
    while(1){
    	// Waiting for 2 seconds to get new sensor data
    	sleep(2);

    	// Getting data from each sensor 
    	getdata(1, cli_sock);		// Getting data from sensor 1
    	getdata(2, cli_sock);		// Getting data from sensor 2
    	getdata(3, cli_sock);		// Getting data from sensor 3
    	getdata(4, cli_sock);		// Getting data from sensor 4
    	getdata(5, cli_sock);		// Getting data from sensor 5
    	
    	// Adding data to csv file
    	csv_file = fopen(filename, "a");
    	fprintf(csv_file, "%d, %s, %s, %s, %s, %s\n", timestamp, sensor_data[0], sensor_data[1], sensor_data[2], sensor_data[3], sensor_data[4]);
    	fclose(csv_file); 	// Closing file because server will be closed by ctrl C,	
    						// Hence to store each data in the file
    	
    	// Incrementing the timestamp
    	timestamp++;
    }
	
	

    return 0;
}
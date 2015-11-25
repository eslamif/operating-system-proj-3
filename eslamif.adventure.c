//CS 344-400, Frank Eslami
//Program 2

#include <stdio.h>
#include <string.h>
#include <unistd.h>	//getcwd, getpid
#include <sys/types.h>	//getpid, mkdir, open
#include <sys/stat.h>	//mkdir, open
#include <fcntl.h>	//open
#include <stdlib.h>	//malloc, rand
#include <time.h>	//time
#include <errno.h>

//Constants
#define numOfRooms 7
#define allowedConn 6

//Global Variables
char * pickedRooms[8] = {0};	//stores 7 random names
char * filesDirPath; 

//Room Connection Nodes to store in room files
struct connectionNodes {
	int roomName;
	int connectedTo[allowedConn];
	int numOfConn;
	char roomType[11];
	char filePath[255];
};

//Room data read from room files
struct roomData {
	char roomName[512];
	char connTo[allowedConn][512];
	int numOfConn;
	char roomType[11];
	char filePath[255];
};

//Initialize struct nodes to be saved to room files
struct connectionNodes connNodes[numOfRooms];

//Initialize struct nodes to be read from room files into the program
struct roomData roomStruct [numOfRooms];

//Function Declarations
char * createFileDirectory();
void pickRandomRooms(char * roomNames[], char * pickedRooms[]);
void createRoomFiles();
void shuffleArrIndexes(int tmpArr[], int arrSize);
void createConnections();
void createRoomTypes();
void saveToFile();
void introduceGame();
void playGame();	
int userSelectRoom(int roomIndex, char userSelection[]);
int validateInput(int roomIndex, char roomName[]);

int main()
{
	srand(time(NULL));		//initialize random seed
	
	//10 room names hard coded
	char * roomNames[] = {"Bliss", "Luxury", "Buzz", "Seasoning", 
		"Solace", "Zest", "Pleasure", "Fun", "Massage", "Surprise"};

	//3 room types hard coded
	char * roomTypes[] = {"START_ROOM", "MID_ROOM", "END_ROOM"};

	//Create directory for room files & obtain path
	filesDirPath = createFileDirectory();		

	//Pick 7 random room names from roomNames[]	
	pickRandomRooms(roomNames, pickedRooms);

	//Create 7 room files
	createRoomFiles(); 
	
	introduceGame();

	playGame();	

	free(filesDirPath);	//free heap allocated by createFileDirectory()
	return 0;
}


/********* Function Definitions *********/
//Create directory as eslami.rooms.<process ID> for room files
//Post: caller must free return value's heap allocation
char * createFileDirectory() {
	//Get current process ID	
	int processID = getpid();	//current process ID
	char processIDStr[6];
	sprintf(processIDStr, "%d", processID);
	
	//Create directory name w/ process ID
	char dirPath[20] = "/eslami.rooms.";
	strncat(dirPath, processIDStr, 20);
	
	//Create new directory path
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));	//current directory path
	strncat(cwd, dirPath, 1024);

	//Create directory
	mkdir(cwd, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	//Store directory path to heap to return to main
	char * str = malloc((sizeof(cwd)/sizeof(char)) * sizeof(char));
	if(str == NULL) return NULL;	

	strncpy(str, cwd, (sizeof(cwd)/sizeof(char)));
	
	return str;
}

//Pick 7 random rooms from roomNames[]
//Input: array of 10 room names
//Input: array to store picked rooms
//Postcondition: random array of 7 room names
void pickRandomRooms(char * roomNames[], char * pickedRooms[]) {
	//Shuffle roomNames[] array indexes	
	int arrSize = 10;
	int tmpArr[arrSize];	
	shuffleArrIndexes(tmpArr, arrSize);
	
	//select the first 7 rooms from the shuffled indexes
	int i, randNum;	
	for(i = 0; i < 7; i++) {
		pickedRooms[i] = roomNames[tmpArr[i]];
	}
}

//Randomely shuffle a set of numbers
//Input: int array
//Input: array size
//Postcondition: input array's indexes are shuffled
void shuffleArrIndexes(int tmpArr[], int arrSize) {
	//Initialize tmp array to its indexes
	int i; 
	for(i = 0; i < arrSize; i++) {
		tmpArr[i] = i;
	}	

	//Shuffle tmpArr[] indexes
	int j, tmp; 
	for(i = 0; i < arrSize; i++) {
		j = rand() % arrSize;
		tmp = tmpArr[i];
		tmpArr[i] = tmpArr[j];
		tmpArr[j] = tmp; 
	}
}

//Create room files
//Input: pickedRooms[] (7 room names) 
//Post: creates 7 files with room data in them
void createRoomFiles() {
	//Shuffle pickedRooms[] to randomize selection
	int shuffledRoomIndexes[numOfRooms];
	shuffleArrIndexes(shuffledRoomIndexes, numOfRooms);

	//Initialize connNodes[] to pickedRooms[] indexes
	int i;
	for(i = 0; i < numOfRooms; i++) {
		connNodes[i].roomName = i;
	}
	
	//Create connections
	createConnections();

	//Create room types
	createRoomTypes();

	//Save rooms to room files
	saveToFile();
}

//Create connections
//Precondition: struct connNodes[] initialized (connections for each pickedRooms[])
//Postcondition: connNodes[] filled with connections for each pickedRooms[] 
void createConnections() {
	#define SR shuffledRooms[h]
	int shuffledRooms[numOfRooms];
	int randNum;
	int i, j, h;	//counters
	int createConnection; 
	int createNewPool;
	
	//Set starting connection of each room to 0
	for(i = 0; i < numOfRooms; i++) {
		connNodes[i].numOfConn = 0;
	}
	
	//Shuffle pickedRooms[] order to randomize connnections
	shuffleArrIndexes(shuffledRooms, numOfRooms);
	h = 0;

	//Determine number of connections for each pickedRooms[]
	randNum = rand() % (7 - 3) + 3;

	//Iterate through each pickedRooms[] to establish connection
	for(i = 0; i < numOfRooms; i++) {
		createConnection = 0;	//set to false
		createNewPool = 0;	//set to false

		if(connNodes[i].numOfConn < 6) {
			createConnection = 1;
		}		
	
		//Create connections
		if(createConnection == 1) {
			h = 0;
			for(j = 0; j < randNum; j++) {
				if(connNodes[SR].numOfConn < 6 && connNodes[SR].roomName > connNodes[i].roomName) {

					connNodes[i].connectedTo[connNodes[i].numOfConn] = SR;
					connNodes[i].numOfConn++;

//					printf("connNeodes[i] = %d\n", connNodes[i].roomName);
//					printf("connNeodes[SR] = %d\n", connNodes[SR].roomName);
//					printf("\n");
					connNodes[SR].connectedTo[connNodes[SR].numOfConn] = connNodes[i].roomName;
					connNodes[SR].numOfConn++;
					h++;
				}
				else
					h++;
			}
			createNewPool = 1;
		}

		//Create new pool of pickedRooms[]
		if(createNewPool == 1) {
			//Shuffle pickedRooms[] order to randomize connnections
			shuffleArrIndexes(shuffledRooms, numOfRooms);
			h = 0;

			//Determine number of connections for each pickedRooms[]
			randNum = rand() % (7 - 3) + 3;
			
		}
	}
	/*
	//TESTING - Print connections for each pickedRoom[]
	printf("-------------------------\n");
	for(i = 0; i < numOfRooms; i++) {
		printf("Room = %d, numOfconn = %d\n", connNodes[i].roomName, connNodes[i].numOfConn);
		for(j = 0; j < connNodes[i].numOfConn; j++) {
			printf("connTo = %d\n", connNodes[i].connectedTo[j]);	
		}
	}		
	printf("\n");	
	*/

	#undef SR
}

//Create room types
//Precondition: struct connNodes[] initialized (connections for each pickedRooms[])
//Postcondition: START_ROOM, MID_ROOM, END_ROOM set to connNodes[]
void createRoomTypes() {
	int randNum, i;
	int startRoom = 0;	//START_ROOM not taken
	int endRoom = 0;	//END_ROOM not taken
	int shuffledRooms[numOfRooms];
	
	//Shuffle pickedRooms[] order to randomize connnections
	shuffleArrIndexes(shuffledRooms, numOfRooms);

	//Set START_ROOM
	strcpy(connNodes[shuffledRooms[0]].roomType, "START_ROOM");
	
	//Set END_ROOM
	strcpy(connNodes[shuffledRooms[1]].roomType, "END_ROOM");
	
	//Set MID_ROOM
	for(i = 2; i < numOfRooms; i++) {
		strcpy(connNodes[shuffledRooms[i]].roomType, "MID_ROOM");
	}

	/*
	//TEST
	for(i = 0; i < numOfRooms; i++) {
		printf("%s\n", connNodes[i].roomType);
	}
	*/	
}

//Save rooms to files
//Precondition: struct connNodes[] initialized (connections for each pickedRooms[])
//Postcondition: each room saved to file 
void saveToFile() {
	int i, j, h;
	char roomFile[200] = {0};
	char tmpName[10] = {0};
	char tmpChar[2] = {0};

	FILE * pFile;
	int file_ptr = 0, tmp_ptr = 0;
	ssize_t nread = 0, nwritten = 0;
	char buffer[512] = {0};
	char buffer_2[512] = {0};
	char * token;

	//Assign room names and create file path
	for(i = 0; i < numOfRooms; i++) {
		file_ptr = 0;

		strcpy(tmpName, "/room_");
		sprintf(tmpChar, "%d", i);
		strcat(tmpName, tmpChar);
		strcat(roomFile, filesDirPath);
		strcat(roomFile, tmpName);

		//create room files
		pFile = fopen(roomFile, "a+");
		if(pFile == NULL) {
			fprintf(stderr, "Could not open %s\n", roomFile);
			perror("Error");
			exit(1);
		}

		//Save room name to file
		strcpy(buffer, "ROOM NAME: ");
		strcat(buffer, pickedRooms[connNodes[i].roomName]);
		strcat(buffer, "\n");
		nwritten = fwrite(buffer, sizeof(char), strlen(buffer), pFile);

		//Save connections to file
		for(j = 0; j < connNodes[i].numOfConn; j++ ) {

			//Save connections
			memset(buffer, 0, sizeof(buffer)/sizeof(char));
			memset(buffer_2, 0, sizeof(buffer_2)/sizeof(char));
			strcpy(buffer, "CONNECTION ");
			sprintf(buffer_2, "%d", (j + 1));
			strcat(buffer, buffer_2);
			strcat(buffer, ": ");
			strcat(buffer, pickedRooms[connNodes[i].connectedTo[j]]);
			strcat(buffer, "\n");
			nwritten = fwrite(buffer, sizeof(char), strlen(buffer), pFile);

		}	

		//Save room types to file
		memset(buffer, 0, sizeof(buffer)/sizeof(char));
		memset(buffer_2, 0, sizeof(buffer_2)/sizeof(char));
		strcpy(buffer, "ROOM TYPE: ");
		strcat(buffer, connNodes[i].roomType);
		strcat(buffer, "\n");
		nwritten = fwrite(buffer, sizeof(char), strlen(buffer), pFile);

		//Read room file data from file
		memset(buffer_2, 0, sizeof(buffer_2)/sizeof(char));
		rewind(pFile);	
		
		j = -1;
		h = 0;
		while(fgets(buffer_2, sizeof(buffer_2), pFile) != NULL) {
			token = strtok(buffer_2, ":\n");
	
			if(strcmp(token, "ROOM NAME") == 0) {
				token = strtok(NULL, ": \n");
				strcpy(roomStruct[i].roomName, token);				

//				printf("Token Name = %s\n", token);
//				printf("Saved Name = %s\n", roomStruct[i].roomName);
			}
			else if(strcmp(token, "ROOM TYPE") == 0) {
				token = strtok(NULL, ": \n");
				strcpy(roomStruct[i].roomType, token);				

//				printf("Token Type = %s\n", token);
//				printf("Saved Type = %s\n", roomStruct[i].roomType);
			}
			else if(token != NULL) {
				h++;
				j++;
				token = strtok(NULL, " \n");
				strcpy(roomStruct[i].connTo[j], token);				
				roomStruct[i].numOfConn = h;

//				printf("Token Conn = %s\n", token);
//				printf("Saved Conn = %s\n", roomStruct[i].connTo[j]);
			}
		}
//		printf("Saved numOfConn = %d\n", roomStruct[i].numOfConn);
//		printf("\n");

		//Close file
		fclose(pFile);	
		
		//clear roomFile[]
		memset(tmpChar, 0, sizeof(tmpChar)/sizeof(char));
		memset(tmpName, 0, sizeof(tmpName)/sizeof(char));
		memset(roomFile, 0, sizeof(roomFile)/sizeof(char));
	}
}

//Introde the game
void introduceGame() {
	printf("Greetings! Welcome to the adventure game! Seven exotic rooms await you that are designed to give your senses pleasure. Make it to the final room and experience ecstasy like never before! \n");

}

//Play through the game
void playGame() {
	int i, j;
	char userSelection[150];	
	int roomIndex;
	int validInput = 0;
	int notWinner = 1;
	int trackRooms[10];
	int trackRoomsCount = 0;

	//Display starting room
	for(i = 0; i < numOfRooms; i++) {
		if(strcmp(roomStruct[i].roomType, "START_ROOM") == 0) {
			roomIndex = i;
		}
	}	


	//Validate user's initial input	
	while(validInput == 0) {
		roomIndex = userSelectRoom(roomIndex, userSelection);	
		validInput = validateInput(roomIndex, userSelection);
		if(validInput == 1) {
			trackRooms[trackRoomsCount] = roomIndex;	
			trackRoomsCount++;
			break;
		}
	}

	//Go to next rooms
	while(notWinner == 1) {
		//Check if user won
		if(strcmp(roomStruct[roomIndex].roomType, "END_ROOM") == 0) {
			notWinner = 0;
			printf("Congratulations, you won!\n");
			printf("It took you %d steps to win!\n", trackRoomsCount);
			printf("You took the following steps:");
			for(i = 0; i < trackRoomsCount; i++) {
				printf(" %s ", roomStruct[trackRooms[i]].roomName);
			}	
			printf("\n");
			break;
		} 
		else {
			//Find next room	
			for(j = 0; j < numOfRooms; j++) {
				if(strcmp(roomStruct[j].roomName, userSelection) == 0) {
					roomIndex = j;
					break;
				}
			} 		
	
			//Validate input
			validInput = 0;
			while(validInput == 0) {
				roomIndex = userSelectRoom(roomIndex, userSelection);	
				validInput = validateInput(roomIndex, userSelection);
				if(validInput == 1) {
					trackRooms[trackRoomsCount] = roomIndex;	
					trackRoomsCount++;
					break;
				}
			}
				

		}	

	}
}

//Display current location and prompt user to select room
int userSelectRoom(int roomIndex, char userSelection[]) {
	int j;
	
	//Display room name
	printf("\nCURRENT LOCATION: %s\n", roomStruct[roomIndex].roomName);

	//Display connections
	printf("POSSIBLE CONNECTIONS: ");
	for(j = 0; j < roomStruct[roomIndex].numOfConn; j++) {
		if(j == (roomStruct[roomIndex].numOfConn - 1)) {
			printf("%s.\n", roomStruct[roomIndex].connTo[j]);
		}
		else {
			printf("%s, ", roomStruct[roomIndex].connTo[j]);
		}
	}	

	//Prompt user to select room
//	printf("WHERE TO? >");
//	scanf("%[^\n]", userSelection);

	do {	
		printf("WHERE TO? >");
		scanf("%s", userSelection);
		while(getchar() != '\n'); }
	while (userSelection < 1);
	
	return roomIndex;
}



//Validate user's room selection
int validateInput(int roomIndex, char userSelection[]) {
	int i;
	int tmpValid;

	for(i = 0; i < roomStruct[roomIndex].numOfConn; i++) {
//		printf("validated = %s\n", roomStruct[roomIndex].connTo[i]);
//		printf("userSelection = %s, conn = %s\n", userSelection, roomStruct[roomIndex].connTo[i]);
		if(strcmp(userSelection, roomStruct[roomIndex].connTo[i]) == 0) {
			tmpValid = 1;
			break;
		}
		else {
			tmpValid = 0;
		}
	}
	if(tmpValid == 1) {
//		printf("returned 1\n");
		return 1;
	}
	else {
		printf("HUH? I DON.T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
//		printf("returned 0\n");
		return 0;
	}
}





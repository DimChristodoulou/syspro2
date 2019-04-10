#include "../inc/global.h"
#include "../inc/functions.h"

//Function that searches a string array for a string, used to search argv[] array.
int strArraySearch(char const *array[], int len, char *delim){
	for(int i = 0; i < len; ++i){
	    if(!strcmp(array[i], delim))
	        return i;
	}
	return -1;
}

char commonDir[50], inputDir[50], mirrorDir[50], logFile[50];
char *myRootFifo;
int clientId, sendFD, rcvFD;

void signal_handler(){
	char idFileName[53];
	sprintf(idFileName, "%s/%d.id", commonDir, clientId);
	//Removes the id file
	remove(idFileName);

	char mirrorFolder[50] = "rm -r ";
	strcat(mirrorFolder, mirrorDir);
	system(mirrorFolder);

	close(sendFD);
	close(rcvFD);
	remove(myRootFifo);

	exit(0);
}

void listdir(char *tempStrSend, int sendFD, const char *name, char mirrorDir[50]){
    DIR *dir;
    struct dirent *entry;
	FILE *fp;
	int sz;
	char c[1024];
	char tempName[1024];
	//printf("TEMPSTRSEEEEND %s NAAAAAAAAAAAAME %s\n", tempStrSend, name);

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			
			char tempStr[1024];
			strcpy(tempStr, path);

			char *tempRestOfPath = strtok(tempStr, "/");
			char *restOfPath = strtok(NULL,"");
			sprintf(tempName, "%s/%s", mirrorDir, restOfPath);
			
			mkdir(tempName, 0777);

            listdir(tempStrSend, sendFD, path, mirrorDir);
        } else {
			sprintf(tempName,"%s/%s",name,entry->d_name);
			fp = fopen(tempName, "r");
			
			char tempStrFile[1024];
			sprintf(tempStrFile, "%s/%s", name,entry->d_name);

			char *tempRestOfPathFile = strtok(tempStrFile, "/");
			char *restOfPathFile = strtok(NULL,"");	
			printf("FILE %s - %s\n",name, mirrorDir);
			sprintf(tempStrSend, "%lu", strlen(restOfPathFile));
			write(sendFD, tempStrSend, 2);

			write(sendFD, restOfPathFile, strlen(restOfPathFile));

			fseek(fp, 0L, SEEK_END);
			sz = ftell(fp);
			fseek(fp, 0L, SEEK_SET);
			fread(c, sz-1, 1, fp);
			fclose(fp);

			sprintf(tempStrSend, "%d", sz);
			write(sendFD, tempStrSend, 4);

			write(sendFD, c, sz);

        }
    }	
	closedir(dir);
}

void transferComplete(){
	printf("Transfer completed successfully\n");
}

int main(int argc, char const *argv[]){

	signal(SIGQUIT, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGUSR1, transferComplete);

	struct stat st = {0};
    int count, bufferSize;
	FILE *commonDirFP, *inputDirFP, *mirrorDirFP, *logFileFP;

    if(argc == 13){
		count = strArraySearch(argv, argc, "-n");
		clientId = atoi(argv[++count]);

		count = strArraySearch(argv, argc, "-c");
		strcpy(commonDir, argv[++count]);

		count = strArraySearch(argv, argc, "-i");
		strcpy(inputDir, argv[++count]);

		count = strArraySearch(argv, argc, "-m");
		strcpy(mirrorDir, argv[++count]);

		count = strArraySearch(argv, argc, "-b");
		bufferSize = atoi(argv[++count]);

		count = strArraySearch(argv, argc, "-l");
		strcpy(logFile, argv[++count]);
	}
	else{
		printf("Error in number of arguments. Exiting.\n");
		exit(0);
	}

	printf("Client ID %d Common Dir %s Input Dir %s Mirror Dir %s Buffer Size %d Log File %s\n",clientId, commonDir, inputDir, mirrorDir, bufferSize, logFile);

	if (stat(commonDir, &st) == -1) {
		printf("%s doesn't exist.. creating\n", commonDir);
		mkdir(commonDir, 0777);
	}
	if (stat(inputDir, &st) == -1) {
		printf("Input Dir %s doesn't exist.. exiting\n", inputDir);
		exit(0);
	}
	if (stat(mirrorDir, &st) == -1) {
		printf("%s doesn't exist.. creating\n", mirrorDir);
		mkdir(mirrorDir, 0777);
	}
	else{
		printf("Mirror dir %s exists... Exiting\n", mirrorDir);
		exit(0);
	}

	char idFileName[53];
	sprintf(idFileName, "%s/%d.id", commonDir, clientId);
	if( access( idFileName, F_OK ) != -1 ){
		//File exists
		printf("File %s already exists... Exiting\n",idFileName);
		exit(0);
	}
	else{
		//File does not exist
		printf("File %s does not exist\n", idFileName);
		FILE *idFile = fopen(idFileName, "w");
		fprintf(idFile, "%d\n", getpid());
		fclose(idFile);
	}
	

	pid_t  pid1, pid2;

	// pid = fork();
	// if (pid == 0) 
	// 	ChildProcess();
	// else 
	// 	ParentProcess();

	checkListNode *root = (checkListNode*)malloc(sizeof(checkListNode));
	root->checked = 1;
	char *tempClientId = (char*)malloc(50*sizeof(char));
	char *tempClientName = (char*)malloc(50*sizeof(char));
	sprintf(tempClientId, "%d", clientId);
	strcpy(root->clientId, tempClientId);	

	checkListNode *temp = root;

	struct dirent *de, *inputDirFileStruct;
    DIR *dr, *inputDirStruct;

	int status, sendInputDirFD, rcvInputDirFD;
	char *tempStr = (char*)malloc(260*sizeof(char));
  	myRootFifo = (char*)malloc(50*sizeof(char));
	char *myInputDirFifo = (char*)malloc(50*sizeof(char));
	char *tempStrRcv = (char*)malloc(50*sizeof(char));
	char *tempStrRcvFileName = (char*)malloc(50*sizeof(char));
	char *tempStrSend = (char*)malloc(50*sizeof(char));
	char *receivedFileName = (char*)malloc(50*sizeof(char));
	char *ptr;
	char fileContents[1024],zeroBytes[2], zero[2]="00", fopenFilePath[1024], otherInputDir[50], mirrorClientPath[1024];
	int fileNameSize, fileSize;

	logFileFP = fopen(logFile,"w+");

	while(1){
		printf("---------------------------------\n");
		dr = opendir(commonDir);
		// opendir returns NULL if it couldn't open directory 
		if (dr == NULL){
			printf("Could not open current directory");
			return 0;
		}
		while ((de = readdir(dr)) != NULL){
			temp = root;
			
			int len = strlen(de->d_name);
			char *lastThree = &de->d_name[len-3];			
			int checkExists = 0;

			if(strcmp(de->d_name, ".") && strcmp(de->d_name, "..")){
				
				//Get only the files ending in .id
				if(!strcmp(lastThree, ".id")){
					
					//Check if we have already edited the clientId contained in the file
					while(temp != NULL){						
						sprintf(tempStr,"%s.id", temp->clientId);
						if(!strcmp(de->d_name, tempStr)){
							//id file exists in list, so don't push it
							//printf("File %s already exists in file list\n",de->d_name);
							checkExists = 1;			
						}			
						temp = temp->next;
					}
					if(checkExists == 0){
						//printf("File %s does not exist in file list\n",de->d_name);
						tempClientName = de->d_name;
						tempClientId = strtok(de->d_name,".");
						pushToCheckList(&root, tempClientId);
					}



					temp = root;
					while(temp != NULL){

						//We have not visited that client ID
						if(temp->checked == 0){

							pid1 = fork();
							if (pid1 == 0){
								//FIRST CHILD PROCESS WITH PIPE TO SEND
								sprintf(tempClientId, "%d", clientId);

								sprintf(mirrorClientPath,"%s/%s",mirrorDir,temp->clientId);
								mkdir(mirrorClientPath, 0777);

								sprintf(myInputDirFifo, "%s/input%s-to-%s.fifo", commonDir,tempClientId, temp->clientId);
								if(mkfifo(myInputDirFifo, 0666) < 0 ){
									//perror("fifo failed!");
								}
								rcvInputDirFD = open(myInputDirFifo, O_RDONLY);
								read(rcvInputDirFD, otherInputDir, 50);
								close(rcvInputDirFD);

								sprintf(myRootFifo, "%s/id%s-to-id%s.fifo", commonDir,tempClientId, temp->clientId);
								//printf("\n%s\n", myRootFifo);
								if(mkfifo(myRootFifo, 0666) < 0 ){
									//perror("fifo failed!");
								}
								sendFD = open(myRootFifo, O_WRONLY);								

								printf("\tOTHERINPUTDIR %s MYROOTFIFO %s\n",otherInputDir, myRootFifo);
								//b.i.1 START
								listdir(tempStrSend, sendFD, otherInputDir, mirrorClientPath);
								write(sendFD, zero, 2);

							}
							else{
								//PARENT PROCESS
								pid2 = fork();
								if (pid2 == 0){
									//SECOND CHILD PROCESS WITH PIPE TO RECEIVE

									sprintf(tempClientId, "%d", clientId);

									sprintf(myInputDirFifo, "%s/input%s-to-%s.fifo", commonDir, temp->clientId, tempClientId);
									if(mkfifo(myInputDirFifo, 0666) < 0 ){
										//perror("fifo failed!");
									}
									sendInputDirFD = open(myInputDirFifo, O_WRONLY);
									write(sendInputDirFD, inputDir, strlen(inputDir)+1);
									close(sendInputDirFD);

									sprintf(myRootFifo, "%s/id%s-to-id%s.fifo", commonDir, tempClientId, temp->clientId);		
									if(mkfifo(myRootFifo, 0666) < 0 ){
										//perror("fifo failed!");
									}
									rcvFD = open(myRootFifo, O_RDONLY);									

									while(read(rcvFD, tempStrRcv, 2)>0){

										//printf("\t %s TEMPSTRRECEIVE %s MYROOTFIFO\n", tempStrRcv, myRootFifo);
										memset(receivedFileName,0,strlen(receivedFileName));
										memset(tempStrRcvFileName,0,strlen(tempStrRcvFileName));
										memset(fileContents,0,strlen(fileContents));

										if(!strcmp(tempStrRcv, "00")){
											printf("AAA \n%s\n",tempStrRcv);
											kill(getppid(), SIGUSR1);											
											//break;
										}
										printf("ONE\n");
										fileNameSize = strtol(tempStrRcv, &ptr, 10);

										read(rcvFD, receivedFileName, fileNameSize);

										read(rcvFD, tempStrRcvFileName, 4);
										fileSize = strtol(tempStrRcvFileName, &ptr, 10);
										//printf("\t RECEIVED FILE SIZE %d\n", fileSize);

										read(rcvFD, fileContents, fileSize);										
										//printf("\t RECEIVED FILE CONTENTS %s\n", fileContents);

										sprintf(fopenFilePath,"%s/%s/%s",mirrorDir,temp->clientId,receivedFileName);
										//printf("\t FILEPAATH %s\n", fopenFilePath);
										FILE *fp = fopen(fopenFilePath,"w");
										fprintf(fp, "%s\n", fileContents);
										fclose(fp);

										fprintf(logFileFP, "Client %d received a file from client %s with name %s and size %d\n", clientId, temp->clientId, receivedFileName, fileSize);

									}
									//wait(&status);
								}
								else{
									//MAIN PARENT PROCESS									
								}
							}
							temp->checked = 1;
						}
						//We have visited that client ID
						else{
							//printf("visited\n");
						}
						if(temp->next == NULL)
							break;
						temp = temp->next;
					}
				}
			}			
		}
		sleep(4);
		closedir(dr);
	}

    return 0;
}

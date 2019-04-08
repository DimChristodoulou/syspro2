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
int clientId;

void signal_handler(){
	char idFileName[53];
	sprintf(idFileName, "%s/%d.id", commonDir, clientId);
	//Removes the id file
	remove(idFileName);

	char mirrorFolder[50] = "rm -r ";
	strcat(mirrorFolder, mirrorDir);
	system(mirrorFolder);

	exit(0);
}

int main(int argc, char const *argv[]){

	signal(SIGQUIT, signal_handler);
	signal(SIGINT, signal_handler);

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
		mkdir(commonDir, 0700);
	}
	if (stat(inputDir, &st) == -1) {
		printf("Input Dir %s doesn't exist.. exiting\n", inputDir);
		exit(0);
	}
	if (stat(mirrorDir, &st) == -1) {
		printf("%s doesn't exist.. creating\n", mirrorDir);
		mkdir(mirrorDir, 0700);
	}
	else{
		printf("Mirror dir %s exists... Exiting\n", mirrorDir);
		exit(0);
	}

	char idFileName[53];
	sprintf(idFileName, "%s/%d.id", commonDir, clientId);
	FILE *idFile = fopen(idFileName, "w");
	fprintf(idFile, "%d\n", getpid());
	fclose(idFile);

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

	struct dirent *de;
    DIR *dr;

	char *tempStr = (char*)malloc(260*sizeof(char));
  	
	while(1){
		printf("---------------------------------\n");
		dr = opendir(commonDir);
		printList(root);
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
						//printf("%s - %s\n",tempStr, de->d_name);
						if(!strcmp(de->d_name, tempStr)){
							//id file exists in list, so don't push it
							printf("File %s already exists in file list\n",de->d_name);
							checkExists = 1;			
						}			
						temp = temp->next;
					}
					if(checkExists == 0){
						printf("File %s does not exist in file list\n",de->d_name);
						tempClientName = de->d_name;
						tempClientId = strtok(de->d_name,".");
						pushToCheckList(&root, tempClientId);
					}



					temp = root;
					while(temp != NULL){
						//We have not visited that client ID
						printf("CHECKED %d\n", temp->checked);
						if(temp->checked == 0){
							pid1 = fork();
							if (pid1 == 0){
								//FIRST CHILD PROCESS
								char *myRootFifo = (char*)malloc(50*sizeof(char));
								sprintf(tempClientId, "%d", clientId);
								sprintf(myRootFifo, "%s/%s-to-%s.fifo", commonDir,tempClientId, temp->clientId);
								printf("\n%s\n", myRootFifo);
								if(mkfifo(myRootFifo, 0666) < 0 ){
									perror("fifo failed!");
								}
								temp->checked = 1;
								//ChildProcess();
							}
							else{
								//PARENT PROCESS
								pid2 = fork();
								if (pid2 == 0){
									//SECOND CHILD PROCESS
									//ChildProcess();
								}
								else{
									//ParentProcess();
								}
							}
						}
						//We have visited that client ID
						else{
							printf("visited\n");
						}
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

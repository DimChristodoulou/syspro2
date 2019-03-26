#include "../inc/global.h"

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

    return 0;
}

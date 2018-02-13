#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#define MAX_INPUT_CHARS 513 /*512 + Null Terminator*/

void interactive();

void batch(char *batchFileName);

char *removePeripheralCharsFromString( char *fullString, char* peripheralChar );

char** splitWithNSizeDelimiter(char* a_str, char* delimChar, int delimMode);

int executeCMD(char *cmdString, char **argVector);

int main(int argc, char *argv[]) {
	if (argc == 1) {
		interactive();
	} else if (argc == 2) {
		batch(argv[1]);
	} else {
		printf("Only less than two arguments supported\n");
	}
	return 0;
}


void interactive() {
	char fullString[MAX_INPUT_CHARS];
	int status=0;
	while (1) {
		printf("papadiakos_8302> ");
		if (fgets(fullString, MAX_INPUT_CHARS, stdin) && (!feof(stdin)) && (!ferror(stdin)) && (strlen(fullString)!=1)){
			char** semicolonCMDs=(splitWithNSizeDelimiter(removePeripheralCharsFromString(removePeripheralCharsFromString(fullString, '\n'), ' ') , ';' , 1));
			for(int i=0;*(semicolonCMDs+i);i++) {
				char** ampersandCMDs=(splitWithNSizeDelimiter(removePeripheralCharsFromString( *(semicolonCMDs+i) , ' ') , '&', 2));
				for(int j=0;*(ampersandCMDs+j);j++) {
					char** cmdArgs=(splitWithNSizeDelimiter(removePeripheralCharsFromString( *(ampersandCMDs+j) , ' ') ,  ' ' , 3));
					if (strcmp(cmdArgs[0], "quit") == 0){
						printf("Execution terminated by 'quit'\n");
						exit(EXIT_SUCCESS);
					}
					status=executeCMD(cmdArgs[0],cmdArgs);
					if (status)break; // Here it's determined that doubleAmpersand delimited commands will fail in cascade form
				}
			}
		}
	}
}

void batch(char *batchFileName){
	FILE* batchFile = fopen(batchFileName, "r");
	if (!batchFile){
		printf("Unable to open file.\n");
		exit(EXIT_FAILURE);
	}
	int status=0;
	char fileLine[MAX_INPUT_CHARS];
	while (fgets(fileLine, sizeof(fileLine) , batchFile)) {
		if ((!feof(batchFile)) && (!ferror(batchFile)) && (strlen(fileLine)>1)){
			char** semicolonCMDs=(splitWithNSizeDelimiter(removePeripheralCharsFromString(removePeripheralCharsFromString(fileLine, '\n'), ' ') , ';' , 1));
			for(int i=0;*(semicolonCMDs+i);i++) {
				char** ampersandCMDs=(splitWithNSizeDelimiter(removePeripheralCharsFromString( *(semicolonCMDs+i) , ' ') , '&', 2));
				for(int j=0;*(ampersandCMDs+j);j++) {
					char** cmdArgs=(splitWithNSizeDelimiter(removePeripheralCharsFromString( *(ampersandCMDs+j) , ' ') ,  ' ' , 3));
					if (strcmp(cmdArgs[0], "quit") == 0){
						printf("Execution terminated by 'quit'\n");
						exit(EXIT_SUCCESS);
					}
					status=executeCMD(cmdArgs[0],cmdArgs);
					if (status)break; // Here it's determined that doubleAmpersand delimited commands will fail in cascade form
				}
			}
		}
	}
}

char *removePeripheralCharsFromString(char *fullString, char* peripheralChar) {
	char periChar[2];
	periChar[0] = peripheralChar;
	periChar[1] = 0;
	//Checking for spaces preluding the command string
	while ((unsigned char) *fullString == *periChar){
		fullString++;
	}
	// String's initial value is the null terminator meaning string was spaces all along
	if(*fullString == 0){
		return fullString;
	}
	//Triming following spaces
	char *endPointer;
	endPointer = fullString + strlen(fullString) - 1;// Initialize endPointer as most "convenient" position::there are no spaces after the command
	while ( (endPointer > fullString) && ((unsigned char) *endPointer == *periChar) ){
		endPointer--; // Decrement endPointer's pointer position
	}
	*(endPointer+1) = 0; // Write a new null terminator on the next pointer position of endPointer value
						 // defining the new end of *string
	return fullString;
}

int executeCMD(char *cmdString, char **argVector){
	int status = 0;
	pid_t pid = fork();
	if (pid < 0) {
		perror("Unable to fork");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		// Child process branch
		if (execvp(cmdString, argVector)) {
			perror("Error executing command");
			_exit(EXIT_FAILURE);
		}
	}
	else {
		// Parent process branch
		while (wait(&status) != pid);
		return status;
	}
}

char** splitWithNSizeDelimiter(char* a_str, char* delimChar, int delimMode) {
	char** result = 0;
	int count = 0;
	char* tmp = a_str;
	char* lastDelimiter = 0;
	char delim[2];
	delim[0] = delimChar;
	delim[1] = 0;

	/* Count how many elements will be extracted. */
	while (*tmp){
		if (delimChar == *tmp){
			count++;
			lastDelimiter = tmp;
			/* Fixing last delimiter position due to existance of second delimiter*/
			switch (delimMode) {
				case 2:
					if(delimChar == *(tmp+1)){
						tmp++;
						lastDelimiter = tmp;
					}
					break;
				case 3:
					while(delimChar == *(tmp+1)){
						tmp++;
						lastDelimiter = tmp;
					}
				break;
				default:;
				//Do Nothing. Single delimiter case is already configured in the first if(){}
			}
		}
		tmp++;
	}

	/* Add space for trailing token. */
	count += lastDelimiter < (a_str + strlen(a_str) - 1);

	/* Add space for terminating null string so caller
	knows where the list of returned strings ends. */
	count++;
	result = malloc(sizeof(char*) * count);

	if ( result ) {
		size_t idx  = 0;
		char* token = strtok(a_str, delim);
		while (token){
			assert(idx < count);
			*(result + idx++) = strdup(token);
			token = strtok(0, delim);
		}
		assert(idx == count - 1);
		*(result + idx) = 0;
	}
	return result;
}

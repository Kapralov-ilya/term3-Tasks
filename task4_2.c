#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

char *generate_file(const char *string)
{
	int length=strlen(string);
	char *filename;
	filename=malloc(length+8);
	memcpy(filename ,string ,length);
	memcpy(filename+length,"_sorted",8);
	filename[length+7]='\0';
	return filename;
}

void copystring(int sourceFile, int destinationFile)
{
	int letter='a',result;
	while (letter!='\n'){
       		result=read(sourceFile,&letter,1);
       		if (result==-1){
       		    perror("read");
			exit(1);
		}
		result=write(destinationFile,&letter,1);
		if (result==-1){
			perror("write");
			exit(1);
		}
	}
}

void sortfile(int sourceFile, int destinationFile, int maxlength)
{
	int counter, letter='a', result, currentlength;
	for(counter=1; counter<=maxlength; counter++){
		lseek(sourceFile,0,0);
		currentlength=0;
		while ((result=read(sourceFile,&letter,1))!=0){
			if (result==-1){
				perror("read");
				exit(1);
			}
			currentlength++;
			if (letter=='\n'){
				if (currentlength==counter){
					lseek(sourceFile,-counter,1);
					copystring(sourceFile,destinationFile);
				}
			currentlength=0;
			}	
		}
	}
}

int main(int argc,char **argv)
{
	int sourceFile,destinationFile,maxlength;
	char *nameFile;
	if (argc!=3){
		fprintf(stderr,"Wrong amount of arguments\n");
		exit(1);
	}
	sourceFile=open(argv[1],O_RDONLY);
	if (sourceFile==-1){
		perror(argv[1]);
		exit(1);
	}
	nameFile=generateFile(argv[1]);
	maxlength=atoi(argv[2]);
	destinationFile=open(nameFile,O_WRONLY|O_TRUNC|O_CREAT,0644);
	if (destinationFile==-1){
		perror(nameFile);
		exit(1);
	}
	sortfile(sourceFile , destinationFile,maxlength);
	close(sourceFile);
	close(destinationFile);
	free(nameFile);
	return 0;
}

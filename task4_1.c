#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define SIZESTRING 10

typedef struct list
{
        char *string;
        int vogue;
        struct list *next;
} list;

char *contentstring()
{
        int stringCounter,sizeCounter,size=SIZESTRING;
        char letter,*stringOfFile=NULL;
        stringCounter=sizeCounter=0;
        letter=getchar();
        while (letter!=EOF && letter!='\n' ){
                if (stringCounter==0){
                        stringOfFile=malloc(size+1);
                }else{
                        if (sizeCounter==SIZESTRING-1){
                                size+=SIZESTRING;
                                stringOfFile=realloc(stringOfFile,size+1);
                                sizeCounter=0;
                        }
                }
                stringOfFile[stringCounter]=letter;
                stringCounter++;
                sizeCounter++;
                letter=getchar();
        }
        if (stringOfFile){
                stringOfFile[stringCounter]='\0';
        }
        if (letter=='\n' && !stringOfFile){
                stringOfFile=malloc(1);
                stringOfFile[0]='\0';
        }
        return stringOfFile;
}

list *addfirst(list *mainlist, char *filestring)
{
        list *tempary;
        tempary=malloc(sizeof(list));
        tempary->string=filestring;
        tempary->vogue=1;
        tempary->next=mainlist;
        return tempary;
}

list *createlist()
{
        int flag;
        char *filestring;
        list *mainlist=NULL,*tempary;
        while ((filestring=contentstring())){
                flag=0;
                tempary=mainlist;
                while (tempary){
                        if (!strcmp(tempary->string,filestring)){
                                tempary->vogue++;
                                flag=1;
                                free(filestring);
                                break;
                        }
                        tempary=tempary->next;
                }
                if (!flag){
                        mainlist=addfirst(mainlist,filestring);
                }
        }
        return mainlist;
}



int searchmax(list *mainlist)
{
        int max=0;
        while (mainlist){
                if ((mainlist->vogue)>max){
                        max=mainlist->vogue;
                }
                mainlist=mainlist->next;
        }
        return max;
}

void freecontent(list *mainlist)
{
        list *tempary;
        while (mainlist){
                free(mainlist->string);
                tempary=mainlist;
                mainlist=mainlist->next;
                free(tempary);
        }
}

int main(int argc,char **argv)
{
        int filedescriptor,maxvogue;
        list *contentFile,*tempary;
        if (argc!=2){
                fprintf(stderr,"wrong arguments\n");
                exit(1);
        }
        filedescriptor=open(argv[1],O_RDONLY|O_CREAT);
        if (filedescriptor==-1){
                perror(argv[1]);
                exit(1);
        }
        dup2(filedescriptor,0);
        contentFile=createlist();
        tempary=contentFile;
        maxvogue=searchmax(contentFile);
        tempary=contentFile;
        while (tempary){
                if ((tempary->vogue)==maxvogue){
                        printf("the most popular string: %s\n",tempary->string);
                }
                tempary=tempary->next;
        }
        freecontent(contentFile);
        setbuf(stdin,NULL);
        setbuf(stdout,NULL);
        close(filedescriptor);
        return 0;
}

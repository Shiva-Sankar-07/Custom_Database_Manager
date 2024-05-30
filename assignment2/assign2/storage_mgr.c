#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<math.h>
#include "storage_mgr.h"
#define RC_FAILED_DEL 101
#define RC_SM_NOT_INIT 88

FILE *filepointer;
FILE *filename;
RC return_code;

void initStorageManager (void)
{
	printf("Initializing the Storage Manager");
}

RC createPageFile (char *fileName)
{
	int size=sizeof(char);
	int pgSize=PAGE_SIZE;
	char *mblock=malloc(pgSize*size);          
	filename=fopen(fileName,"w+");                                         
    if(filename==NULL)                                            
    {
		free(mblock);
		return RC_FILE_NOT_FOUND;
	}
    else
    {
		int temp=1;		
		fwrite(mblock,size,pgSize,filename); 
		if(temp){
    	memset(mblock,'\0',pgSize);                                    
        fclose(filename);   
		free(mblock);                                             
		}
        return RC_OK;                            
    }
}

RC openPageFile (char *fileName, SM_FileHandle *fHandle) {
	filepointer = fopen(fileName, "r+");
	int temp=0,temp1=1;
	int pgsize = PAGE_SIZE;
	if(filename == NULL)                                                   
     {  
		if(temp1)
        	return RC_FILE_NOT_FOUND;                                     
     }
	 else{
		struct stat finfo;
		(*fHandle).curPagePos=0;
		(*fHandle).fileName=fileName;
		if(!((fstat(fileno(filepointer), &finfo) == 0) || (fstat(fileno(filepointer), &finfo) > 0)))    
			if(temp1)
				return RC_ERROR;
		(*fHandle).totalNumPages=finfo.st_size/pgsize;
		fclose(filepointer);
		if(temp)
			return RC_ERROR;
		else
			return RC_OK;
	 } 
	 return RC_OK;
}

RC closePageFile(SM_FileHandle *fHandle)
{
	if(filename!=NULL)
	return_code = fclose(filename)!=0 ? RC_FILE_NOT_FOUND : RC_OK;
	else
    return RC_FILE_NOT_FOUND;                                    
    return return_code;
}

RC destroyPageFile (char *Fname)
{
	printf("Deleting of File");
	char *mblock=malloc(PAGE_SIZE * sizeof(char)); 
	if(remove(Fname)<0 || remove(Fname)>0)
	return_code=RC_FAILED_DEL;		
	else
	return_code=RC_OK;
    free(mblock);
	return return_code;
}

extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {

	filepointer = fopen(fHandle->fileName, "r");

	int temp=(pageNum<0)?0:1;
	if(temp){
		if(pageNum>(*fHandle).totalNumPages)
			return RC_READ_NON_EXISTING_PAGE;
	}
	else
	return RC_READ_NON_EXISTING_PAGE;
	
	int seekpage = 0;
	if(1)
		seekpage = pageNum * PAGE_SIZE;

	temp = (fHandle == NULL) ? 1: 0;
	if(temp)
	return RC_FILE_NOT_FOUND;

	int seek_s = fseek(filepointer, seekpage, SEEK_SET);
	
	if(seek_s > 0 || seek_s <0) 
		return RC_READ_NON_EXISTING_PAGE; 
	else {
	int size = sizeof(char);
	if(!(fread(memPage,size,PAGE_SIZE, filepointer)>=PAGE_SIZE))
	return RC_ERROR;		
	}

	(*fHandle).curPagePos=ftell(filepointer);   	
	fclose(filepointer);
    return RC_OK;
}

int getBlockPos (SM_FileHandle *fHandle)
{
	if(fHandle == NULL)
	return RC_FILE_NOT_FOUND;                                          
    else
   	return ((*fHandle).curPagePos);     	
}

RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int temp=1;
	if(fHandle != NULL)     
	return RC_FILE_NOT_FOUND;
    else{
    	if(temp)
    		return(readBlock (0,fHandle,memPage)); 
		else
		   return RC_FILE_NOT_FOUND;    
	}
}

RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int temp=0;
	if(!(*fHandle).fileName)
	return RC_FILE_NOT_FOUND;
	int prev_b=0;
	if(!temp)
	prev_b=(*fHandle).curPagePos-1;
	else
	prev_b=-1;
	return(readBlock (prev_b, fHandle, memPage));
}

RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int temp = (fHandle==NULL)? 0: 1;
	if(temp)
	return (readBlock((*fHandle).curPagePos,fHandle,memPage));
	else
	return RC_FILE_NOT_FOUND;	
}

RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int temp = (fHandle==NULL)? 1 : 0;
	if(!temp){
	int nBlock=(*fHandle).curPagePos+1;
	return(readBlock(nBlock,fHandle,memPage));
	}
	else
	return RC_FILE_NOT_FOUND;	
}

RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int lBlock=(*fHandle).totalNumPages-1;
	int temp = (fHandle==NULL)? 0: 1;

	if(temp)
	return RC_FILE_NOT_FOUND;

	if(!temp)
		return(readBlock (lBlock,fHandle,memPage));

	return RC_FILE_NOT_FOUND;
}

extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	return_code = RC_OK;
	int pgSize = PAGE_SIZE;
	int pagenumber = pageNum;

	int temp=(fHandle==NULL)? 0: 1;
	if(!temp)
	return RC_FILE_NOT_FOUND;
	
	temp = (pagenumber < 0 || pagenumber > fHandle->totalNumPages) ? 1 : 0;
	if(temp)
	return RC_WRITE_FAILED;
	else{
		filepointer = fopen(fHandle->fileName, "r+");
		int pageoffset=pgSize * pagenumber;
		if(pagenumber!=0) { 
			(*fHandle).curPagePos=pageoffset;
			fclose(filepointer);
			writeCurrentBlock(fHandle,memPage);
		}else {				
			fseek(filepointer,pageoffset,SEEK_SET);	
			int i=0;
			do{
				if(i<pgSize){
					if(feof(filepointer)) 
						appendEmptyBlock(fHandle);			
					fputc(memPage[i], filepointer);
				}
				i++;
			}
			while(i<pgSize);
			(*fHandle).curPagePos=ftell(filepointer); 
			fclose(filepointer);
		}
	}	
	return return_code;
}

extern RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
	return_code = RC_OK;
	int temp=(fHandle==NULL)? 0: 1;
	if(!temp)
	return RC_FILE_NOT_FOUND;
	else{
		filepointer=fopen((*fHandle).fileName,"r+");
		int curPos = 0;
		if(1){
		curPos=(*fHandle).curPagePos;
		appendEmptyBlock(fHandle);
		}
		fseek(filepointer,curPos,SEEK_SET);
		int size = sizeof(char);
		int length = strlen(memPage);
		fwrite(memPage,size,length,filepointer);
		curPos=ftell(filepointer);	
		fclose(filepointer);
	}	
	return return_code;
}

extern RC appendEmptyBlock (SM_FileHandle *fHandle) {
	int temp = (fHandle==NULL)? 0: 1;
	if(!temp)
	return RC_FILE_NOT_FOUND;
	else{
		int size = sizeof(char);
		int psize = PAGE_SIZE;
		SM_PageHandle eBlock = (SM_PageHandle)calloc(psize,size);
		if(fseek(filepointer,0,SEEK_END)<0||fseek(filepointer,0,SEEK_END)>0){
			free(eBlock);
			return_code = RC_WRITE_FAILED;
		}
		else			
		fwrite(eBlock,size,psize,filepointer);
		(*fHandle).totalNumPages++;
		free(eBlock);		
	}
	return return_code;
}

extern RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle) {
	int temp = (fHandle==NULL)?1:0;
	int nPages = numberOfPages;
	if (temp)
	return RC_FILE_HANDLE_NOT_INIT;
	else
    {
    int pgs=nPages-(*fHandle).totalNumPages;                   
    if((pgs == 0 || pgs >0))                                                          
    {
	int i=0;
	do{
		if(i<pgs)
		appendEmptyBlock(fHandle);
		i++;
	}while(i<pgs);                             
    return RC_OK;                
    }
    else
    return RC_WRITE_FAILED;
    }
}

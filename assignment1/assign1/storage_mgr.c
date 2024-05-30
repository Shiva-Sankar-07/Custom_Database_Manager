/*
storage_mgr.c
*/

// Including the header files required for running the code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "dberror.h"
#include "storage_mgr.h"

#define FILE_REPERMISSIONS "r+"
FILE *myfile_object;

#define FILEWRITE_CHECKER(__stmt__) if (-1 == __stmt__) { return RC_WRITE_FAILED; }
#define FILE_CHECKER(__pointer__, __error__) if (!__pointer__) { return __error__; }
#define FILEHANDLE_CHECKER(fh) if (!fh) { return RC_FILE_HANDLE_NOT_INIT; }

/**************  Manipulating the page files ************/

/* As per the assignment requirement, we have 5 functions to manipulate the page files. They are:
void initStorageManager (void);
RC createPageFile (char *fileName);
RC openPageFile (char *fileName, SM_FileHandle *fHandle);
RC closePageFile (SM_FileHandle *fHandle);
RC destroyPageFile (char *fileName);
*/

void initStorageManager (void){

//This function is used to initialize the storage manager

printf("\n Initializing the storage manager.......");
printf("\n A20517528 \t Shiva Sankar Modala");
printf("\n");

printf("\n Storage manager initialized successfully");

}

/**
* createPageFile
* ==============
* function parameters:
*    fileName: the filename of the page file.
* This function is used to create a new page file fileName. 
* The created file initial size should be one page. 
* This function fills this single page with '\0' bytes.
* return:
*    The status of the file creation.
*    If the file failed to create, return RC_FILE_HANDLE_NOT_INIT,
*    If the file failed to write, return RC_WRITE_FAILED.
 **/
 
 RC createPageFile (char *fileName){
 
 FILE_CHECKER(fileName, RC_FILE_NOT_FOUND);
 int len = sizeof(char);
 RC final_return;
 
 myfile_object = fopen(fileName, "w+");
 char *block_of_memory = calloc(PAGE_SIZE, len);
 memset(block_of_memory,'\0',PAGE_SIZE);
 
 int check = fwrite(block_of_memory, len, PAGE_SIZE, myfile_object);
 //final_return = FILEWRITE_CHECKER(fwrite(block_of_memory, len, PAGE_SIZE, myfile_object));
 
 final_return = (check == 0) ? RC_WRITE_FAILED : RC_OK;
 
 free(block_of_memory);
 fclose(myfile_object);
 return final_return;
}


/**
 *openPageFile
 *============
 *function parameters:
 *   fileName: the filename of the page file.
 *   fHandle: the handle to be created by this function.
 *This function opens an existing page file. 
 *This function returns RC_FILE_NOT_FOUND if the file does not exist.
 *If file opening is successful, then the fields of this file handle should be initialized with the information about the opened file. 
 *For instance, you have to store the total number of pages that are stored in the file from disk.
 *return:
 *    If the file does not exist, return RC_FILE_NOT_FOUND
**/

RC openPageFile (char *fileName, SM_FileHandle *fHandle){

 FILE_CHECKER(fileName, RC_FILE_NOT_FOUND);
 FILEHANDLE_CHECKER(fHandle);
 
 RC final_return; 
 myfile_object = fopen(fileName, "r+");
 
 if(myfile_object != NULL){
   fseek(myfile_object, 0, SEEK_END);
   fHandle -> fileName = fileName;
   //page_count = (ftell(myfile_object) + 1) / PAGE_SIZE;
   fHandle -> totalNumPages = (int)(ftell(myfile_object) + 1) / PAGE_SIZE;
   fHandle -> curPagePos = 0;
   fHandle -> mgmtInfo = myfile_object;
   rewind(myfile_object);
   final_return = RC_OK;
 }
 else
   final_return = RC_FILE_NOT_FOUND;
 
 return final_return;
}
 
/**
* closePageFile
* =============
* parameters:
*     fHandle: handle of the file to be closed.
*  return:
*    The status of the file closed.
*  This function is used to close an open page file.
**/

RC closePageFile (SM_FileHandle *fHandle){

 FILEHANDLE_CHECKER(fHandle);
 RC final_return;
 int check = fclose(fHandle -> mgmtInfo);
 if(check == 0)
   final_return = RC_OK;
 else
   final_return = RC_FILE_NOT_FOUND;
   
 return final_return;
}

/**
* destroyPageFile
* =============
* parameters:
*     fileName: name of the file to be destroyed.
*  return:
*    The status of the file destroyed.
*  This function is used to destroy(delete) a page file.
**/

RC destroyPageFile (char *fileName){

 FILE_CHECKER(fileName, RC_FILE_NOT_FOUND);
 RC final_return;
 int check = remove(fileName);
 if(check == 0)
   final_return = RC_OK;
 else
   final_return = RC_FILE_NOT_FOUND;
   
 return final_return;
}

/**************  reading blocks from disc ************/

/* As per the assignment requirement, we have 7 functions to read blocks from the disk. They are:
RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage);
int getBlockPos (SM_FileHandle *fHandle);
RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
*/

/**
* readBlock
* =========
* function parameters:
*     pageNum: page number to be read in file
*     fHandle: file handle of the file
*     memPage: memory block
*
* This function reads the pageNumth block from a file and stores its content in the memory pointed to by the memPage page handle. 
* return:
* 	If the file has less than pageNum pages, the function should return RC_READ_NON_EXISTING_PAGE.
**/
RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
 
 FILEHANDLE_CHECKER(fHandle);
 
 if(fHandle -> mgmtInfo == NULL)
   return RC_FILE_NOT_FOUND;
 int temp = 0;
 if(pageNum >= fHandle -> totalNumPages || pageNum < 0)
   return RC_READ_NON_EXISTING_PAGE;
 
 int len = sizeof(char);
 temp++;
 
 RC final_return;
 
 int check = fread(memPage, len, PAGE_SIZE, fHandle -> mgmtInfo);
 fseek(fHandle -> mgmtInfo, PAGE_SIZE * pageNum, SEEK_SET);
 if(!(check < 0) || !(check > PAGE_SIZE)){
    temp++;
    fHandle -> curPagePos = pageNum;
    final_return = RC_OK;
 }
 
 if(temp != 2)
 return RC_READ_NON_EXISTING_PAGE;
 
 return final_return;
}

/**
* getBlockPos
* ===========
* function parameters:
*    fHandle: file handle of the file
* return:
*    Return the current page position in a file.
* This function is used to get the block position in the file.
**/

int getBlockPos(SM_FileHandle *fHandle) {
    //int block_pos = fHandle -> curPagePos;
    return fHandle -> curPagePos;
}

/*
* readFirstBlock
* ==============
* function parameters:
*    fHandle: file handle of the file
*    memPage: memory block
* return:
*    Return the status of first block read.
* This function is used to read the first page in the file.
**/

RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
    //RC final_return = readBlock(0, fHandle, memPage);
    return readBlock(0, fHandle, memPage);
}

/**
* readPreviousBlock
* ==============
* function parameters:
*    fHandle: file handle of the file
*    memPage: memory block
* return:
*    Return the status of previous block read.
* This function is used to find previous block & read it's content using present blocks position.
**/

RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  int pagePos = getBlockPos(fHandle);   
  pagePos--;
  RC final_return = readBlock(pagePos, fHandle, memPage);                                            
  return final_return;    
}

/**
* readCurrentBlock
* ==============
* function parameters:
*    fHandle: file handle of the file
*    memPage: memory block
* return:
*    Return the status of current block read.
* This function to read contents using the present blocks position.
**/

RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  int pagePos = getBlockPos(fHandle);   
  RC final_return = readBlock(pagePos, fHandle, memPage);                                            
  return final_return;    
}

/**
* readNextBlock
* ==============
* function parameters:
*    fHandle: file handle of the file
*    memPage: memory block
* return:
*    Return the status of current block read.
* This function is used to find next block & read it's content using present blocks position.
**/

RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  int pagePos = getBlockPos(fHandle);   
  pagePos++;
  RC final_return = (pagePos > fHandle -> totalNumPages) ? RC_READ_NON_EXISTING_PAGE : RC_OK;                                           
  return final_return;    
}

/**
* readLastBlock
* ==============
* function parameters:
*    fHandle: file handle of the file
*    memPage: memory block
* return:
*    Return the status of first block read.
* This function is used to read the first page in the file.
**/

RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage) {
    int block_pos = fHandle->totalNumPages - 1;
    RC final_return = readBlock(block_pos, fHandle, memPage);
    return final_return;
}

/**************  writing blocks to a page file ************/

/* As per the assignment requirement, we have 4 functions to write blocks to a page. They are:
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage);
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
RC appendEmptyBlock (SM_FileHandle *fHandle);
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle);
*/

/**
* writeBlock
* ==========
* function parameters:
*     pageNum: page number to be read in file
*     fHandle: file handle of the file
*     memPage: memory block
*
* This function is used to write content into specified page
**/

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
 
 FILEHANDLE_CHECKER(fHandle);
 
 if(fHandle -> mgmtInfo == NULL)
   return RC_FILE_NOT_FOUND;
 
 int temp = 0;
 if(pageNum > fHandle -> totalNumPages || pageNum < 0)
   return RC_WRITE_FAILED;
 
 int len = sizeof(char);
 temp++;
 
 RC final_return;
 
 if(fseek(fHandle -> mgmtInfo, PAGE_SIZE * pageNum, SEEK_SET) == 0){
   int check = fwrite(memPage, len, PAGE_SIZE, fHandle -> mgmtInfo);
   if(check != 0){
      temp++;
      fHandle -> curPagePos = pageNum;
      fseek(fHandle -> mgmtInfo, 0, SEEK_END);
      fHandle -> totalNumPages = ftell(fHandle -> mgmtInfo);
      final_return = RC_OK;
      temp = 500;
   }
 }
 
 if(temp != 500)
 final_return = RC_WRITE_FAILED;
 
 return final_return;
}

/**
* writeCurrentBlock
* =================
* function parameters:
*    fHandle: file handle of the file
*    memPage: memory block
* return:
*    Return the status of current block write.
* This function is used to write data into block taking current position of the pointer.
**/

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  int pagePos = getBlockPos(fHandle);   
  RC final_return = readBlock(pagePos, fHandle, memPage);                                            
  return final_return;    
}

/**
* appendEmptyBlock
* =================
* function parameters:
*    fHandle: file handle of the file
* This function to append an empty block.
**/

RC appendEmptyBlock (SM_FileHandle *fHandle){
  
  FILEHANDLE_CHECKER(fHandle);
  
  int len = sizeof(char);
  char *block_of_memory = calloc(PAGE_SIZE, len);
  RC final_return;
  int check =  fwrite(block_of_memory, len, PAGE_SIZE, fHandle -> mgmtInfo);
  
  //final_return = FILEWRITE_CHECKER(fwrite(block_of_memory, len, PAGE_SIZE, fHandle -> mgmtInfo));
  
  if(check != 0){
    fHandle -> totalNumPages++;
    final_return = RC_OK;
  }
  else
    final_return = RC_WRITE_FAILED;
  free(block_of_memory);
  
  return final_return;
}

/**
* ensureCapacity
* ==============
* function parameters:
*    numberOfPages: number of pages.
*    fHandle: file handle of the file
* This function increase number of pages in the file if the file has pages less than numberOfPages parameter.
**/

RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle){
  FILEHANDLE_CHECKER(fHandle);
  
  RC final_return;
  
  while(fHandle -> totalNumPages < numberOfPages){
    appendEmptyBlock(fHandle);
  }
  final_return = RC_OK;
  
  return final_return;
}
/*
buffer_mgr.c
*/

// Including the header files required for running the code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"

//int initval = 0;
int lfu_pointer = 0;
int buffer_length = 0;
int clock_pointer = 0;
int last_Index = 0;
int hit = 0;
int page_count = 0;
int count = 0;


/*--------------- Buffer Manager Interface Pool Handling ------------------*/

/* As per the assignment requirement, we have 3 functions in buffer management interface pool handling. They are:
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData);
RC shutdownBufferPool(BM_BufferPool *const bm);
RC forceFlushPool(BM_BufferPool *const bm);
*/

/**
* initBufferPool
* --------------
* creates a new memory buffer pool.
* sets the buffer's node count to 1, indicating that no pages are accessible in the buffer.
* numPages specifies the buffer size, i.e. the number of page frames that may be stored in the buffer.
* pageFileName option holds the name of the page file whose pages are cached in memory.
* strategy denotes the page replacement technique (FIFO, LRU, LFU, CLOCK) that this buffer pool will employ.
* stratData is used to pass parameters to the page replacement strategy, if any exist.
**/

RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData)
{
  RC final_return;
  if(bm == NULL){
    final_return = RC_BUFF_POOL_NOT_FOUND;
    return final_return;
  } 
  else{
    bm->pageFile=(char *)pageFileName;
    bm->strategy=strategy;
    bm->numPages=numPages;
    if(pageFileName == NULL){
	final_return = RC_FILE_NOT_FOUND;
	return final_return;
    }
    else{
      lfu_pointer = 0;
      int frame_size = sizeof(PageFrame);
      clock_pointer = 0;
      int buffer = numPages * frame_size;
      buffer_length = 0;
      PageFrame *pg_frame = malloc(buffer);
      count = 0;
      buffer_length = numPages;
      final_return = RC_OK;
      
      int a = 0;
      while(a < buffer_length){
        pg_frame[a].fixCount = 0;
        pg_frame[a].data = NULL;
        pg_frame[a].dirtyPage = 0;
        pg_frame[a].pageNum = -1;
        pg_frame[a].index = 0;
        pg_frame[a].hitNum = 0;
        a++;
      }
      bm -> mgmtData = pg_frame;
    }
  } 
  return final_return;
}   
  
/**
* shutdownBufferPool
* ------------------
* destroys a buffer pool. This method should free up all resources associated with buffer pool. 
* For example, it should free the memory allocated for page frames. 
* If the buffer pool contains any dirty pages, then these pages should be written back to disk before destroying the pool. 
* It is an error to shutdown a buffer pool that has pinned pages.
 **/
  
RC shutdownBufferPool(BM_BufferPool *const bm)
{
  RC final_return;
  if(bm == NULL){
    final_return = RC_BUFF_POOL_NOT_FOUND;
    return final_return;
  }
  else{
  final_return = RC_OK;
  PageFrame *pg_frame;
  pg_frame = (PageFrame *)(*bm).mgmtData;
  int a = 0;
  int res = forceFlushPool(bm);
  
  if(res == final_return){
    do{
      if(pg_frame[a].fixCount != 0){
        final_return = RC_PINNED_PAGES_IN_BUFFER;
        return final_return;
      }
      a++;
    }while(!(a > buffer_length || a == buffer_length));
    (*bm).mgmtData = NULL;
    free(pg_frame);
  }
  else{
    final_return = RC_WRITE_BACK_FAILED;
    return final_return;
  }
  }
  return RC_OK;
}
  
/**
* forceFlushPool
* --------------
* used to shutdown, or delete, the buffer pool and set all of its values to null.
* Before removing the buffer pool, we execute forceFlushPool(...), which writes all dirty (modified) pages to disc.
* If any user accesses any page, the RC PINNED PAGES IN BUFFER error is thrown.
**/

extern RC forceFlushPool(BM_BufferPool *const bm)
{
  PageFrame *pg_frame;
  RC final_return;
  int a = 0;
  pg_frame = (PageFrame *)(*bm).mgmtData;
  if(bm == NULL){
    final_return = RC_BUFF_POOL_NOT_FOUND;
    return final_return;
  }
  final_return = RC_OK;
  while(a < buffer_length)
  {
    if(pg_frame[a].fixCount == 0){
      if(pg_frame[a].dirtyPage == 1){
        SM_FileHandle file;
        int error = 0;
        if(openPageFile((*bm).pageFile, &file) == RC_OK){
          //printf("File Opening Error");
        }
        if(1){
          writeBlock(pg_frame[a].pageNum, &file, pg_frame[a].data);
        }
        pg_frame[a].dirtyPage = error;
        if(a < buffer_length)
          count++;
      }
    }
    a++;
  }
  return final_return;
}      


/*---------------- Buffer Manager Interface Access Pages ----------------*/
/* As per the assignment requirement, we have 4 functions in buffer manager interface access pages. They are:
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page);
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page);
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page);
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum);
*/
 
/**
* markDirty
* ---------
* used to flag a certain page as dirty if the newly changed content has not yet been written back to the file.
**/

RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{
  RC final_return;
  if(bm == NULL){
    final_return = RC_BUFF_POOL_NOT_FOUND;
    printf(" The buffer pool doesnot exist ");
    return final_return;
  }
  final_return = RC_OK;
  if(page == NULL){
    final_return = RC_PAGE_ERROR;
    printf("Invalid Page");
    return final_return;
  }
  PageFrame *pg_frame;
  final_return = RC_OK;
  pg_frame = (PageFrame *)(*bm).mgmtData;
  if(1){
  int a = 0;
  while(a < buffer_length){
    int cnt = page -> pageNum;
    if(!(pg_frame[a].pageNum < cnt)){
      if(!(pg_frame[a].pageNum > cnt)){
	   pg_frame[a].dirtyPage = 1;
	   return final_return;
	 }
    }
    else{
      if((pg_frame[a].pageNum == cnt)){
	   pg_frame[a].dirtyPage = 1;
	   return final_return;
	 }
    }
    a++;
  }
  }
  final_return = RC_ERROR;
  return final_return;
}
  
/**
* unpinPage
* ---------
* If the page is discovered in the buffer that was previously pinned, this function unpins it.
* decreases the fixCount by one, indicating that the client is no longer accessing this page. 
* Otherwise, an error is thrown.
**/

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{	
  RC final_return;
  PageFrame *pg_frame;
  int a = 0;
  pg_frame = (PageFrame *)(*bm).mgmtData;
  if(bm == NULL){
    final_return = RC_BUFF_POOL_NOT_FOUND;
    printf("The Buffer poool not exists");
    return final_return;
  }
  if(page == NULL){
    final_return = RC_PAGE_ERROR;
    printf("Invalid Page");
    return final_return;
  }
  final_return = RC_OK;
  while(a < buffer_length){
    if(!(a > buffer_length || a == buffer_length)){
    int cnt = page -> pageNum;
    int temp = pg_frame[a].fixCount;
    if(pg_frame[a].pageNum != cnt){
      pg_frame[a].fixCount = temp;
    }
    else{
      pg_frame[a].fixCount = temp - 1;
    }
    }
    a++;
  }
  return final_return;
}

/**
* forcePage
* ---------
* writes a page back to the disc file. 
* Even if it is dirty, it will force it to save to the file and make the required modifications to the parameters.
* It writes the page back to the file on disc using the Storage Manager services.
* It sets dirtyBit = 0 for that page after writing.
**/

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
  RC final_return;
  if(bm == NULL){
    final_return = RC_BUFF_POOL_NOT_FOUND;
    return final_return;
  }
  int buffer = 0;
  final_return = RC_OK;
  PageFrame *pg_frame;
  int a = 0;
  pg_frame = (PageFrame*)(*bm).mgmtData;
  while(!(a > buffer_length || a == buffer_length)){
    if(!(a > buffer_length || a == buffer_length)){
      int cnt = page -> pageNum;
      if(!(pg_frame[a].pageNum < cnt || pg_frame[a].pageNum > cnt)){
        SM_FileHandle file;
	   if(openPageFile((*bm).pageFile, &file) != final_return){
	     printf("File opening error");
	   }
	   if(1){
	   writeBlock(pg_frame[a].pageNum, &file, pg_frame[a].data);
	   }	
	   count++;			
	   pg_frame[a].dirtyPage = buffer;	
      }
    }
    a++;
  }
  return final_return;
}

/*----------------- Page Replacement Algorithms ------------------*/
/**
* The page replacement strategy functions implement the FIFO, LRU, LFU, and CLOCK algorithms for pinning a page. 
* If the buffer pool is full and a new page has to be pinned, use a page from the buffer pool instead. 
* These page replacement techniques specify which buffer pool pages must be replaced.
* We implemented four page replacement algorithms. They are:
* FIFO - First In First Out
* LRU - Least Recently Used
* CLOCK
* LFU - Least Frequently Used
**/

/**
* FIFO
* ----
* The most basic page replacement approach is first in, first out (FIFO).
* We keep a global integer variable that is increased whenever a page is changed. 
* (global variable) % buffer_length allows us to specify which page must be erased at each round.
* FIFO is similar to a queue in that the first page in the buffer pool is at the front, and if the buffer pool is full, the first page in the buffer pool is replaced.
**/

void FIFO(BM_BufferPool *const bm, PageFrame *page)
{
  PageFrame *pg_frame;
  int pos;
  if(!(bm == NULL)){
    pg_frame = (PageFrame *)(*bm).mgmtData;
    int a = 0;
    pos = last_Index % buffer_length;
    while(a < buffer_length){
      if(!(pg_frame[pos].fixCount != 0)){
        int temp = 1;
        if(!(pg_frame[pos].dirtyPage != temp)){
          SM_FileHandle file;
          openPageFile((*bm).pageFile, &file);
	     writeBlock(pg_frame[pos].pageNum, &file, pg_frame[pos].data);
	     count += 1;
	   }
	   pg_frame[pos].data = (*page).data;
	   pg_frame[pos].fixCount = (*page).fixCount;
	   pg_frame[pos].hitNum = (*page).hitNum;
	   pg_frame[pos].pageNum = (*page).pageNum;
	   pg_frame[pos].dirtyPage = (*page).dirtyPage;
	   break;
	 }
	 else{
	   pos += 1;
	   if(pos % buffer_length == 0)
	     pos = 0;
	 }
	 a++;
    }
  }
}

/**
* LFU
* ---
* Least Frequently Used (LFU) eliminates the page frame in the buffer pool that is used the fewest often (the fewest number of times) compared to the other page frames.
* A global integer variable keeps track of how many times each page in memory has been accessed. 
* The page with the fewest references is eliminated.
**/

void LFU(BM_BufferPool *const bm, PageFrame *page)
{
  PageFrame *pg_frame;
  int index = lfu_pointer;
  pg_frame = (PageFrame*)(*bm).mgmtData;
  int leastFrequentReference = 0;
  int k = 0,b = 0;
  block1:
  if(!(k > buffer_length || k == buffer_length)){
    if(!(pg_frame[index].fixCount != 0)){
      index = (index + k) % buffer_length;
      leastFrequentReference = pg_frame[index].index;
    }
  }
  k += 1;
  if(!(k > buffer_length || k == buffer_length))
    goto block1;

  k = index + 1 + 0;
  k = k % buffer_length;

  block2:
  if(pg_frame[k].index < leastFrequentReference){
    if(!(b > buffer_length || b == buffer_length)){
      index = k;
      leastFrequentReference = pg_frame[k].index;
    }
    k += 1;
    k = k % buffer_length;
  }
  b += 1;
  if(!(b > buffer_length || b == buffer_length))
    goto block2;

  int temp = 1;
  if(pg_frame[index].dirtyPage == temp)
  {
    SM_FileHandle file;
    int tp = 1;
    openPageFile((*bm).pageFile, &file);
    temp--;
    writeBlock(pg_frame[index].pageNum, &file, pg_frame[index].data);
    if(1){
      count += 1;
    }
    tp = count - 1;
  }
		
  pg_frame[index].data = page->data;
  pg_frame[index].dirtyPage = page->dirtyPage;
  pg_frame[index].pageNum = page->pageNum;			
  pg_frame[index].fixCount = page->fixCount;
  lfu_pointer = index + 1;	
}

/**
* LRU
* ---
* Least Recently Used (LRU) removes the page frame that hasn't been used in a long time (the least recent) from the buffer pool.
* A global integer variable allows us to keep track of the order in which the pages arrived.
**/

void LRU(BM_BufferPool *const bm, PageFrame *page)
{
  PageFrame *pg_frame; 
  bool flag = true;
  int a = 0;
  pg_frame = (PageFrame *) (*bm).mgmtData;
  int lefthead, head;
  int temp = 0;
  block1:
  if(!(a > buffer_length || a == buffer_length)){
    if(!(pg_frame[a].fixCount != temp)){
      head = pg_frame[a].hitNum;
      lefthead = a;
      flag = false;
    }
  }
  a += 1;
  if(!(a > buffer_length || a == buffer_length) && flag)
    goto block1;
		

  a = 1 + 0 + lefthead;
  block2:
  if(pg_frame[a].hitNum < head){
    if(!(a > buffer_length || a == buffer_length)){
      head = pg_frame[a].hitNum;
      lefthead = a;
    }
  }

  a += 1;
  if(!(a > buffer_length || a == buffer_length))
    goto block2;

  int temp2 = 1;
  if(!(pg_frame[lefthead].dirtyPage != temp2))
  {
    SM_FileHandle file;
    count += 1;
    openPageFile((*bm).pageFile, &file);
    writeBlock(pg_frame[lefthead].pageNum, &file , pg_frame[lefthead].data);		
  }
  int temp3 = lefthead;
  pg_frame[temp3].data = page->data;
  pg_frame[temp3].fixCount = page->fixCount;
  pg_frame[temp3].pageNum = page->pageNum;
  pg_frame[temp3].hitNum = page->hitNum;		
  pg_frame[temp3].dirtyPage = page->dirtyPage;
}

/**
* CLOCK
* -----
* The CLOCK algorithm keeps track of the last page frame added to the buffer pool. 
* In addition, to point the page frames in the buffer pool, we utilise a clockPointer, which is a counter.
* A clock pointer is kept that points to the frame where the replacement algorithm begins. 
* Another variable is toggled between 1 and 0, and if this variable is 1, this page is given a second chance, and the value is toggled to 0 and the pointer is incremented to continue looking for a page to replace. 
* If the variable value is 0 and the page is the first to arrive, it gets replaced.
**/

void CLOCK(BM_BufferPool *const bm, PageFrame *page)
{	
  PageFrame *pg_frame;
  int temp = 0;
  pg_frame = (PageFrame *)  (*bm).mgmtData;
  if(!(bm != NULL)){
  while(1){
  if(clock_pointer % buffer_length != 0)
    continue;
  else 
    clock_pointer = 0;
  if(pg_frame[clock_pointer].hitNum < temp || pg_frame[clock_pointer].hitNum > temp){
    pg_frame[clock_pointer].hitNum = 0;
    clock_pointer += 1;
  }
  else{
    int statclk;
    int temp1 = 1;
    SM_FileHandle file;
    if((pg_frame[clock_pointer].dirtyPage == temp1)){
      statclk = openPageFile(bm->pageFile, &file);
      if(statclk)
      count = count + 0;
      writeBlock(pg_frame[clock_pointer].pageNum, &file, pg_frame[clock_pointer].data);
      count++;
    }

    pg_frame[clock_pointer].data = (*page).data;
    pg_frame[clock_pointer].fixCount = (*page).fixCount;
    pg_frame[clock_pointer].hitNum = (*page).hitNum;
    pg_frame[clock_pointer].pageNum = (*page).pageNum;
    pg_frame[clock_pointer].dirtyPage = (*page).dirtyPage;
    clock_pointer += 1;

    break;	
  }
  }
  }
}


/**
* pinPage
* -------
* This function serves two primary purposes. 
* Return the requested page to the user if it is available in the buffer. 
* If the buffer is full, use strategy to acquire the page from the file, add it to the buffer, or replace it with any other node, and then return it to the user.
* We have implemented the FIFO, LRU, LFU, and CLOCK page replacement techniques when pinning a page.
* Page replacement algorithms identify which pages must be replaced. 
* If a page is found to be unclean, it is checked. 
* If dirtyBit = 1, the contents of the page frame are written to the page file on disc, and the new page is inserted where the previous page was.
**/
        

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{
  RC final_return;
	
  if(bm == NULL){
    final_return = RC_BUFF_POOL_NOT_FOUND;
    return final_return;
  }
  else{
    if(page == NULL){
      final_return = RC_PAGE_ERROR;
      return final_return;
    }
    else{
      PageFrame *pg_frame;
      int temp = -1;
      pg_frame = (PageFrame *)(*bm).mgmtData;
      if(pg_frame[0].pageNum != temp)
      {
        int a = 0;
        bool isBufferFull = true;
        bool flag = true;	

        block1:
        if(pg_frame[a].pageNum == temp){
          if(!(a > buffer_length || a == buffer_length)){
            SM_FileHandle file;
            int size = PAGE_SIZE;
            openPageFile((*bm).pageFile, &file);
            pg_frame[a].data = (SM_PageHandle) malloc(size);
            int zero = 0;
            pg_frame[a].pageNum = pageNum;
            int cnt = 1;	
            readBlock(pageNum, &file, pg_frame[a].data);
            last_Index++;	
            pg_frame[a].fixCount = cnt;
            hit += 1; 
            pg_frame[a].index = zero;					
            	
		 					
            bool ss = true;
            if(ss == true){
            if(RS_LRU==(*bm).strategy){
              pg_frame[a].hitNum=1;
            }
            ss = false;
            }
            if(ss == true){
            if(RS_LRU == (*bm).strategy){
              pg_frame[a].hitNum = hit;
            }
            ss = false;
            }
            page->data = pg_frame[a].data;
            isBufferFull = false;
            flag = false;
            page->pageNum = pageNum;
							
          }
        }
        else{
          if(!(a > buffer_length || a == buffer_length)){
            if(pg_frame[a].pageNum == pageNum){
              isBufferFull = false;
              pg_frame[a].fixCount +=  1;
              hit += 1; 
              bool fg = true;
              if(fg == true){
              if((*bm).strategy == RS_CLOCK){
                pg_frame[a].hitNum = 1;
              }
              fg = false;
              }
              if(fg == true){
              if((*bm).strategy == RS_LRU){
                pg_frame[a].hitNum = hit;
              }
              }
              if(fg == true){
              if((*bm).strategy == RS_LFU){
                pg_frame[a].index++;
              }
              }

              page->data = pg_frame[a].data;
              flag = false;
              page->pageNum = pageNum;
              clock_pointer += 1;						
	       }
	     }
	   }
	   a += 1;
        if((!(a > buffer_length || a == buffer_length)) && flag)
          goto block1;


        if(isBufferFull){
          PageFrame *newpg;
          SM_FileHandle file;
          int cst = sizeof(PageFrame);
          newpg = (PageFrame*) malloc (cst);
          openPageFile((*bm).pageFile, &file);
          newpg->data = (SM_PageHandle) malloc (PAGE_SIZE);
          readBlock(pageNum, &file,(*newpg).data);
          last_Index += 1;
          if(flag){
            newpg->dirtyPage = 0;
            newpg->pageNum = pageNum;
            newpg->index = 0;
            newpg->fixCount = 1;
          }
          hit += 1;
          bool ms = true;
          if(ms){
          if(RS_CLOCK == (*bm).strategy){
            (*newpg).hitNum = 1;
          }
          ms = false;
          }
          if((*bm).strategy == RS_LRU){
            (*newpg).hitNum = hit;
          }
          page->pageNum = pageNum;
          page->data = newpg->data;
          while(1){
            if((*bm).strategy == RS_LRU){
              LRU(bm, newpg);
              break;
            }
            else if((*bm).strategy == RS_FIFO){
              FIFO(bm, newpg);
              break;
            }
            else if((*bm).strategy == RS_CLOCK){
              CLOCK(bm, newpg);
              break;
            }
            else if((*bm).strategy == RS_LFU){
              LFU(bm, newpg);
              break;
            }
            else
              break;
            break;
          }
        }
        final_return = RC_OK;
        return final_return;					
      }
      else{
        int sz = PAGE_SIZE;
        SM_FileHandle file;
        openPageFile(bm->pageFile, &file);
        int zr = 0;
        pg_frame[0].data = (SM_PageHandle) malloc (sz);
        last_Index = zr;
        ensureCapacity(pageNum, &file);
        hit = zr;
        readBlock(pageNum, &file, pg_frame[0].data);
        pg_frame[0].fixCount += 1;
        pg_frame[0].pageNum = pageNum;
        pg_frame[0].index = zr;
        pg_frame[0].hitNum = hit;	
        page->data = pg_frame[0].data;				
        page->pageNum = pageNum;				
        final_return = RC_OK;
        return final_return;					
      }	
    }
  }	
}


/*---------------------- Statics Interface -------------------------*/
/* As per the assignment requirement, we have 5 functions in statics interface. They are:
PageNumber *getFrameContents (BM_BufferPool *const bm);
bool *getDirtyFlags (BM_BufferPool *const bm);
int *getFixCounts (BM_BufferPool *const bm);
int getNumReadIO (BM_BufferPool *const bm);
int getNumWriteIO (BM_BufferPool *const bm);
*/

/**
* getFrameContents
* ----------------
* returns a list of all the contents of the buffer pool pages. 
* The size of the list equals the size of the buffer (numPages).
* We loop through all of the page frames in the buffer pool to determine the pageNum value of the page frames in the buffer pool.
* The page number of the page saved in the 'n'th page frame is the 'n'th element.
**/

PageNumber *getFrameContents (BM_BufferPool *const bm)
{
  int sz = sizeof(PageNumber);
  PageFrame *pg_frame;
  int tp = 0, a = 0;
  int size = sz * buffer_length;
  PageNumber *cnt = malloc(size);
  int zr = 0;
  pg_frame = (PageFrame *)(*bm).mgmtData;
  int ng = -1;
  block1:
  if(!(pg_frame[a].pageNum < ng || pg_frame[a].pageNum > ng)){
    cnt[a] = NO_PAGE;
  }
  else{
    if(tp == zr)
      cnt[a] = pg_frame[a].pageNum;
  }
  a++;
	
  if(!(a > buffer_length || a == buffer_length))
    goto block1;
  return cnt;
}

/**
* getDirtyFlags
* -------------
* This method produces a list of all the filthy pages that are currently stored in the buffer pool. 
* The list size equals the buffer size (numPages).
* We loop through all of the page frames in the buffer pool to obtain the dirtyBit value of the page frames in the buffer pool.
* If the page saved in the 'n'th page frame is filthy, the 'n'th element is TRUE.
**/

bool *getDirtyFlags (BM_BufferPool *const bm)
{
  PageFrame *pg_frame;
  int a = 0;
  int sz = sizeof(bool);
  int size = sz * buffer_length;	
  pg_frame = (PageFrame *)(*bm).mgmtData;	
  bool *dtfg = malloc(size);
  int tp = 1;
		
  block1:
  if(!(size == 0)){
    if(pg_frame[a].dirtyPage < tp || pg_frame[a].dirtyPage > tp){
      dtfg[a] = false;
    }

    if(pg_frame[a].dirtyPage == tp){
      dtfg[a] = true;
    }	
  }
  a += 1;

  if(!(a > buffer_length || a == buffer_length))
    goto block1;
  return dtfg;
}

/**
* getFixCounts
* ------------
* This method produces a list of the fix counts for all the pages in the buffer pool. 
* The length of the list equals the length of the buffer (numPages).
* We loop over all of the page frames in the buffer pool to acquire the fixCount value of the page frames in the buffer pool.
* The fixCount of the page saved in the 'n'th page frame is the 'n'th element.
**/

int *getFixCounts (BM_BufferPool *const bm)
{
  int sz = sizeof(int);	
  PageFrame *pg_frame;
  int buffer = 0;
  pg_frame = (PageFrame *)(*bm).mgmtData;
  buffer = sz * buffer_length;
  int *fixcnt = malloc(buffer);
  int temp = 0, a =0;
  int tp = -1;
  int zr = 0;

  if(!(buffer < 0 || buffer == 0)){
  while(a < buffer_length){
    if(a < buffer_length){
      if(!(pg_frame[a].fixCount < tp || pg_frame[a].fixCount > tp)){
        if(temp == zr)
          fixcnt[a] = 0;
      }
      else{
        fixcnt[a] = pg_frame[a].fixCount;
      }
    }
    a++;
  }
  }
  return fixcnt;
}

/**
* getNumReadIO
* ------------
* This method returns the total number of IO read counts done by the buffer pool, i.e. the number of disc pages read.
* We keep track of this information with the rearIndex variable.
**/

int getNumReadIO (BM_BufferPool *const bm)
{
	if(!(bm == NULL)){
		last_Index += 1;
		int res = last_Index;
		if(true){
		return res;
		}
	}
	else{
		return last_Index++;
	}
}

/**
* getNumWriteIO
* -------------
* This method returns the total number of IO write counts performed by the buffer pool, i.e. the number of disc pages written.
* The writeCount variable is used to keep track of this information. 
* When the buffer pool is created, we set writeCount to 0 and increase it everytime a page frame is written to disc.
**/

int getNumWriteIO (BM_BufferPool *const bm)
{
	int res = count;
	if(true){
	return res;
	}
}
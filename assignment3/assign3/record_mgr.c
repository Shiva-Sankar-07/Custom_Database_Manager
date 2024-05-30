/*
record_mgr.c
*/

// Including the header files required for running the code
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "storage_mgr.h"
#include "dberror.h"
#include "record_mgr.h"
#include <fcntl.h>
#include "buffer_mgr.h"


//global variable
const int ATTRIBUTE_SIZE = 15;
RC final_return;

//defining RecordManager struct for storing records
typedef struct RecordManager
{
	RID rcd_ID;
	int cnt_TP;
	BM_PageHandle buff_pgs_hdl;	
	Expr *cndt;
	int cnt_scn;
	BM_BufferPool buff_poool;	
	int free_pgs;
} Recd_Mngr;

Recd_Mngr *rcod_Mangr;

//custom function to find the number of free slots available
int freeSltCount(char *val, int rcdsize)
{
	int ttl;
	int temp = 0;
	int cst;
	ttl = PAGE_SIZE / rcdsize; 
	a:
	cst = temp * rcdsize;
	if(!(val[cst]=='+'))
		return temp;
	temp+=1;
	if(!(temp == ttl||temp>ttl))
		goto a;
	return -1;
}

/*---------------table and manager-----------------*/

/* As per the assignment requirement, we have 7 functions in table and manager. They are:
RC initRecordManager (void *mgmtData);
RC shutdownRecordManager ();
RC createTable (char *name, Schema *schema);
RC openTable (RM_TableData *rel, char *name);
RC closeTable (RM_TableData *rel);
RC deleteTable (char *name);
int getNumTuples (RM_TableData *rel);
*/

/**
* initRecordManager
* -----------------
* This function is used to initialize the record manager.
* We use the Storage Manager's initStorageManager(...) function to initialize the storage Manager. 
**/

RC initRecordManager (void *mgmtData)
{
	//RC final_return;
	bool val = true;
	final_return = RC_OK;
	if(val==true){
		printf("\n Initializing record manager ... \n");
		initStorageManager();
	}
	return final_return;
}

/**
* shutdownRecordManager
* ---------------------
* This function is used to shut down the record manager and de-allocate all of the record manager's resources.
* This function frees up all of the Record Manager's resources and memory space. 
* We free up memory by setting the recordManager data structure pointer to NULL by making use of inbuilt C function free ().
**/

RC shutdownRecordManager ()
{
	bool val=true;
	//RC final_return;
	//Recd_Mngr *rcod_Mangr;
	final_return = RC_OK;
	if(val != false){
		shutdownBufferPool(&rcod_Mangr->buff_poool);
		rcod_Mangr=NULL;
		free(rcod_Mangr);
	}
	return final_return;
}

/**
* createTable
* -----------
* This function is used to create new table. It displays the table with the name specified by the parameter name.
* This function calls initBufferPool to initialize the Buffer Pool (...). We decided to make use of LRU page replacement policy, even though there are other replacement algorithms.
* This function configures the table's attributes (name, datatype, and size) as well as all of the table's values. 
* this function then creates a page file, opens it, writes the table block to the page file, and closes it.

**/

RC createTable (char *name, Schema *schema)
{
	//Recd_Mngr *rcod_Mangr;
	//RC final_return;
	int sz = sizeof(Recd_Mngr);
	final_return = RC_OK;
	char k[PAGE_SIZE];
	rcod_Mangr = (Recd_Mngr*) malloc(sz);
	//bool val = FALSE;
	int temp = 100;
	char *pg_temp = k;
	do{

		initBufferPool(&rcod_Mangr->buff_poool, name, temp, RS_LRU, NULL);
		int b = sizeof(int);
		*(int*)pg_temp = 0; 
		pg_temp += b;
		*(int*)pg_temp = 1;
		pg_temp += b;
		*(int*)pg_temp = (*schema).numAttr;
		pg_temp += b; 
		*(int*)pg_temp = (*schema).keySize;
		pg_temp += b;

	}while(FALSE);
	
	int i = 0;
	//val = TRUE;
	block: 
		strncpy(pg_temp, (*schema).attrNames[i], ATTRIBUTE_SIZE);
		if(TRUE){
				pg_temp += ATTRIBUTE_SIZE;
				*(int*)pg_temp = (int)(*schema).dataTypes[i];
				pg_temp += sizeof(int);
		}
		i+=1;
		*(int*)pg_temp = (int) (*schema).typeLength[i];
		pg_temp += sizeof(int);
		if(i< schema->numAttr){
			goto block;
		}

		SM_FileHandle file;
		int cst = 0;
		if(createPageFile(name) == final_return)
			openPageFile(name, &file);
		
		if(writeBlock(cst, &file, k) == final_return)
			closePageFile(&file);
	return final_return;
}

/**
* openTable
* ---------
* This function creates a table with the name specified by the parameter 'name' in the schema specified by the parameter'schema.' 
* It initializes the buffer pool before pinning a page, reading the schema from the file, and unpinning the page.
**/

RC openTable (RM_TableData *rel, char *name)
{
	bool val=true;
	//RC final_return;
	if((*rel).mgmtData == NULL)
		val = true;
	//Recd_Mngr *rcod_Mangr;
	final_return = RC_OK;
	if(val==false)
		return RC_BUFFER_ERROR;

	if(val != false){
		SM_PageHandle pgs_hdl;    
		(*rel).mgmtData = rcod_Mangr;
		int attb_cnt;
		int buff_sz=sizeof(int);
		(*rel).name = name;
		int a = 0;
		if(buff_sz==sizeof(int)){
			pinPage(&rcod_Mangr->buff_poool, &rcod_Mangr->buff_pgs_hdl, a);
			pgs_hdl = (char*)(*rcod_Mangr).buff_pgs_hdl.data;
		}
		(*rcod_Mangr).cnt_TP= *(int*)pgs_hdl;
		pgs_hdl += buff_sz;
		int sz = sizeof(int);
		if(buff_sz==sz){
			(*rcod_Mangr).free_pgs = *(int*) pgs_hdl;
			pgs_hdl += buff_sz;
		}
		attb_cnt = *(int*)pgs_hdl;
		pgs_hdl += buff_sz;
		int sz1 = sizeof(Schema);
		Schema *schma;
		if(buff_sz==sz){
			int sz2 = sizeof(DataType);
			schma = (Schema*) malloc(sz1);
			int sz5 = attb_cnt;
			(*schma).dataTypes = (DataType*) malloc(sz2 *sz5);			
		}
		int sz9 = attb_cnt;
		(*schma).attrNames = (char**) malloc(sizeof(char*) *sz9);
		(*schma).numAttr = sz9;
		(*schma).typeLength = (int*) malloc(sz *sz9);

		int k = 0;
		int sz3 = ATTRIBUTE_SIZE;
		block:
		(*schma).attrNames[k] = (char*) malloc(sz3);
		++k;
		if(k<attb_cnt)
			goto block;

		int d=0;
		block1:
		strncpy((*schma).attrNames[d],pgs_hdl,sz3);
		pgs_hdl += ATTRIBUTE_SIZE;
		(*schma).dataTypes[d]= *(int*) pgs_hdl;
		pgs_hdl += sz;
		(*schma).typeLength[d]=*(int*)pgs_hdl;
		pgs_hdl += sz;
		++d;
		if(d<(*schma).numAttr)
			goto block1;
		
		final_return = RC_OK;
		bool val1 = true;
		(*rel).schema = schma;
		if(val1 == true && (*rel).schema == schma){
			unpinPage(&rcod_Mangr->buff_poool,&rcod_Mangr->buff_pgs_hdl);
			forcePage(&rcod_Mangr->buff_poool,&rcod_Mangr->buff_pgs_hdl);
		}

	}
	
	
	return final_return;
}

/**
* closeTable
* ----------
* This function is used to close the table specified by the parameter'rel'. 
* It accomplishes this by invoking the Buffer Manager function shutdownBufferPool (...). 
* Before closing the buffer pool, the buffer manager writes the table changes to the page file.
**/

RC closeTable (RM_TableData *rel)
{	
	bool val=false;
	if(rel->mgmtData!=NULL)
		val = true;
	RC final_return;
	if(!val){
		return RC_BUFFER_ERROR;
	}
	final_return = RC_OK;
	if(val==true){
		Recd_Mngr *rcod_Mangr=(*rel).mgmtData;
		final_return = RC_OK;
		shutdownBufferPool(&rcod_Mangr->buff_poool);
	}
	return final_return;
}

/**
* deleteTable
* -----------
* This function deletes the table with the name specified by the parameter 'name.' 
* This function invokes the Storage Manager's destroyPageFile function (...). 
* The destroyPageFile(...) function deletes the page from disk and frees up the memory space reserved for that mechanism. 
* The page will not be deleted if destroyPageFile(...) is not RC_OK.
**/

RC deleteTable (char *name)
{
	bool val=true;
	if(name==NULL){
		val=false;
	}
	RC final_return;
	if(!val)
		return RC_BUFFER_ERROR;
	final_return = RC_OK;
	if(val)
		destroyPageFile(name);
	return final_return;
}

/**
* getNumTuples
* ------------
* This function returns the number of tuples in the table specified by the parameter'rel'. 
* This function returns the value of the variable [tuplesCount] defined in our custom data structure for storing table meta-data.
**/

int getNumTuples (RM_TableData *rel)
{
	bool val=false;	
	int cnt=0;
	if(rel->mgmtData!=NULL)
		val = true;
	if(!val){
		return RC_BUFFER_ERROR;
	}
	if(val==true){
		Recd_Mngr *rcod_Mangr = rel->mgmtData;
		cnt = rcod_Mangr->cnt_TP;
	}	
	return cnt;
}

/*---------------handling records in a table-----------------*/

/* As per the assignment requirement, we have 4 functions in handling records in a table. They are:
RC insertRecord (RM_TableData *rel, Record *record);
RC deleteRecord (RM_TableData *rel, RID id);
RC updateRecord (RM_TableData *rel, Record *record);
RC getRecord (RM_TableData *rel, RID id, Record *record);
*/

/**
* insertRecord
* ------------
* This function inserts a new record into the table and updates the'record' parameter with the Record ID passed in by insertRecord(). 
* We assign a Record ID to the record that is being inserted. 
* We pin the page with an open slot. When we find an empty slot, we locate the data pointer and add a '+' to indicate that a new record has been added. 
* After inserting the record, we mark the page dirty so that the Buffer Manager can write the page's content back to disk. 
* We use the inbuilt memcpy() C function to copy the record's data (passed through parameter'record') into the new record, and then the page is unpinned.
**/

RC insertRecord (RM_TableData *rel, Record *record)
{
	char *dat;
	bool val=true;
	if((*rel).mgmtData ==NULL)
		val = false;

	if(val==false)
		return RC_SCHEMA_ERROR;
	
	if(val==true){
		Recd_Mngr *rcod_Mangr = (*rel).mgmtData;
		RID *recdid = &record->id;
		int rec_sz;			
		rec_sz = getRecordSize((*rel).schema);
		do{
		(*recdid).page = (*rcod_Mangr).free_pgs;
		if(val)
			pinPage(&rcod_Mangr->buff_poool, &rcod_Mangr->buff_pgs_hdl, (*recdid).page);
		dat = (*rcod_Mangr).buff_pgs_hdl.data;
		}
		while(val!=true);
		(*recdid).slot = freeSltCount(dat, rec_sz);

		int tmp = -1;
		while((*recdid).slot == tmp)
		{
			unpinPage(&rcod_Mangr->buff_poool,&rcod_Mangr->buff_pgs_hdl);	
			(*recdid).page++;
			if(val==true)
				pinPage(&rcod_Mangr->buff_poool,&rcod_Mangr->buff_pgs_hdl,(*recdid).page);		
			dat=(*rcod_Mangr).buff_pgs_hdl.data;
			if(val==true)
				(*recdid).slot = freeSltCount(dat, rec_sz);
		}
		char *tp;
		tp=dat;
		if(true&&tp==dat){
			markDirty(&rcod_Mangr->buff_poool,&rcod_Mangr->buff_pgs_hdl);
		}

		if(tp==dat){
		   tp+=((*recdid).slot*rec_sz);
		  *tp='+';
		}

		++tp;
		int tep = rec_sz - 1;
		memcpy(tp,(*record).data + 1, tep);
		//RC final_return;
		unpinPage(&rcod_Mangr->buff_poool,&rcod_Mangr->buff_pgs_hdl);
		if(val){
			(*rcod_Mangr).cnt_TP++;
			int a = 0;
			pinPage(&rcod_Mangr->buff_poool,&rcod_Mangr->buff_pgs_hdl,a);
		}
		final_return = RC_OK;

	}	
	return RC_OK;
}

/**
* deleteRecord
* ------------
* This function is used to delete a record having Record ID 'id' passed through the parameter from the table referenced by the parameter 'rel'.
* We set the freePage meta-data in our table to the Page ID of the page whose record is to be deleted, so that this space can subsequently be used by a new record.
* We pin the page and browse to the record's data pointer, where we change the first character to '-,' indicating that the record has been removed and is no longer required. 
* Finally, we unpin the page and designate it dirty so that the Buffer Manager can store the page's contents back to disk.
**/

RC deleteRecord (RM_TableData *rel, RID id)
{
	//RC final_return;
	bool val=true;
	if(rel->mgmtData == NULL){
		val=false;
	}

	if(val==false){
		final_return = RC_SCHEMA_ERROR;
		return final_return;
	}

	if(val==true){
		final_return = RC_OK;
		int identity;
		Recd_Mngr *rcod_Mangr=rel->mgmtData;
		identity=id.page;
		int recd_sz;
		pinPage(&rcod_Mangr->buff_poool,&rcod_Mangr->buff_pgs_hdl,identity);
		char *data;
		(*rcod_Mangr).free_pgs=id.page;
		recd_sz=getRecordSize((*rel).schema);
		data=rcod_Mangr->buff_pgs_hdl.data;
		identity=0;
		int tep;
		block: 
		tep=id.slot * recd_sz;
		data[tep + identity] = '-';
		++identity;
		if(identity<recd_sz)
			goto block;
		
		int er=identity;
		if(er>=recd_sz){
			markDirty(&rcod_Mangr->buff_poool, &rcod_Mangr->buff_pgs_hdl);
			unpinPage(&rcod_Mangr->buff_poool, &rcod_Mangr->buff_pgs_hdl);
			forcePage(&rcod_Mangr->buff_poool, &rcod_Mangr->buff_pgs_hdl);
		}
	}
	return final_return;
}

/**
* updateRecord
* ------------
* This function updates a record in the table specified by the parameter "record," which is referred to by the argument "rel." 
* It uses the meta-data of the table to find the page where the record is located and pins that page to the buffer pool. 
* It assigns a Record ID and navigates to the data storage location for the record. 
* We use the memcpy() C function to copy the data from the record (provided via parameter'record') into the new record, mark the page dirty, and then unpin the page.
**/

RC updateRecord (RM_TableData *rel, Record *record)
{	
	bool val=true;
	//RC final_return;
	if(rel->mgmtData == NULL){
		val=false;
	}
	if(!val){
		final_return = RC_SCHEMA_ERROR;
		return final_return;
	}
	else{
		final_return = RC_OK;
		int identity;
		Recd_Mngr *rcod_Mangr = (*rel).mgmtData;
		identity=record->id.page;
		char *dat;
		int recd_sz;
		pinPage(&rcod_Mangr->buff_poool, &rcod_Mangr->buff_pgs_hdl,identity);
		recd_sz = getRecordSize(rel->schema);
		int total_sz;
		RID id=record->id;
		dat=(*rcod_Mangr).buff_pgs_hdl.data;
		total_sz = id.slot*recd_sz;
		dat +=total_sz;
		*dat='+';
		++dat;
		recd_sz-=1;
		if(dat>0){
			memcpy(dat,(*record).data+1,recd_sz);
			markDirty(&rcod_Mangr->buff_poool, &rcod_Mangr->buff_pgs_hdl);
			unpinPage(&rcod_Mangr->buff_poool, &rcod_Mangr->buff_pgs_hdl);
		}
	}
	return final_return;	
}

/**
* getRecord
* ---------
* This function retrieves a record from the table specified by "rel," which is also specified in the argument, using the Record ID "id" as a parameter. The recorded data is saved in the location specified by the "record" option. 
* It locates the record's page using the table's meta-data and then pins that page in the buffer pool using the record's 'id'. 
* It sends the data and sets the Record ID of the'record' parameter to the id of the record on the page. 
* After that, the page is unpinned.
**/

RC getRecord (RM_TableData *rel, RID id, Record *record)
{
	//RC final_return;
	bool val=true;
	if((*rel).mgmtData ==NULL){
		val=false;
	}
	if(val==false){
		final_return = RC_SCHEMA_ERROR;
		return final_return;
	}
	if(val){
		final_return = RC_OK;
		Recd_Mngr *rcod_Mangr = (*rel).mgmtData;
		int recd_sz;
		pinPage(&rcod_Mangr->buff_poool,&rcod_Mangr->buff_pgs_hdl, id.page);
		char *dat_pointer;
		recd_sz = getRecordSize((*rel).schema);
		int ttl_sz;
		char *data;
		dat_pointer = (*rcod_Mangr).buff_pgs_hdl.data;
		int tp = 0;
		ttl_sz = id.slot * recd_sz;
		dat_pointer += ttl_sz;
		if(recd_sz !=tp ){
			if(*dat_pointer == '+')
			{
				(*record).id = id;
				data = (*record).data;
				++data;
				memcpy(data, dat_pointer + 1, recd_sz - 1);
			}
			else
			{
				final_return = RC_RM_NO_TUPLE_WITH_GIVEN_RID;
				return final_return;
			}			
		}
		unpinPage(&rcod_Mangr->buff_poool, &rcod_Mangr->buff_pgs_hdl);
	}
	return RC_OK;
}

/*---------------scans-----------------*/

/* As per the assignment requirement, we have 3 functions in scans. They are:
RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond);
RC next (RM_ScanHandle *scan, Record *record);
RC closeScan (RM_ScanHandle *scan);
*/

/**
* startScan
* ---------
* The startScan() function initiates a scan by reading data from the RM ScanHandle data structure, which is passed as an argument. 
* We create the scan-related variables in our one-of-a-kind data structure. 
* If the condition is NULL, the error code RC_SCAN_CONDITION_NOT_FOUND is returned.
**/

RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *condition)
{
	bool val = true;
	Recd_Mngr *tbl_Mngr;
	//RC final_return;
	Recd_Mngr *scn_Mngr;
	if(condition != NULL){
		final_return = RC_OK;
		openTable(rel, "ScanTable");
		int sz=sizeof(Recd_Mngr);
    		scn_Mngr=(Recd_Mngr*)malloc(sz);
		if(val==true){
			if(condition!=NULL)	{
				scan->mgmtData=scn_Mngr;
				int tp=0;
				scn_Mngr->rcd_ID.slot=tp;
				tp=1;
				if(val&&condition!=NULL)
					scn_Mngr->rcd_ID.page=tp;	
				scn_Mngr->cndt=condition;
				tp=0;
				if(val)
					scn_Mngr->cnt_scn=tp;
				if(condition!=NULL)
					tbl_Mngr = (*rel).mgmtData;
				tbl_Mngr->cnt_TP=ATTRIBUTE_SIZE;
				(*scan).rel= rel;
			}
		}
	}	
	else{
		final_return = RC_SCAN_CONDITION_NOT_FOUND;
		return final_return;
	}
	return RC_OK;
}

/**
* next
* ----
* This function returns the next tuple that meets the condition (test expression). 
* If condition is NULL, we return the error code RC_SCAN_CONDITION_NOT_FOUND. 
* If there are no tuples in the table, the error code RC RM NO MORE TUPLES is returned. 
* We loop through the tuples in the table. To evaluate the test expression, pin the page with that tuple, navigate to the data location, copy data into a temporary buffer, and then use eval (....) 
* If the result of the test expression (v.boolV) is TRUE, the tuple meets the requirement. Following that, the page is unpinned, and RC_OK is returned. 
* The error code RC_RM_NO_MORE_TUPLES is returned if none of the tuples satisfy the criterion.
**/

RC next (RM_ScanHandle *scan, Record *record)
{
	bool val=true;
	//RC final_return;
	if((*scan).mgmtData == NULL)
		val=true;

	if(val){
		Schema *schma = (*scan).rel->schema;
		int sz=sizeof(Value);
		Recd_Mngr *tbl_Mngr = (*scan).rel->mgmtData;
		final_return = RC_OK;
		Recd_Mngr *scn_Mngr = (*scan).mgmtData;				
		Value *rslt = (Value *) malloc(sz);	
		
		if((*scn_Mngr).cndt==NULL&&val)
		{
			final_return = RC_SCAN_CONDITION_NOT_FOUND;
			return final_return;
		}
	
		if((*scn_Mngr).cndt!=NULL){
			int rcd_sz = getRecordSize(schma);
			int ttl_slts;
			int sc_cnt;
			int tp_cnt;
			char *dat;
			ttl_slts = PAGE_SIZE / rcd_sz;
			tp_cnt = (*tbl_Mngr).cnt_TP;
			sc_cnt = (*scn_Mngr).cnt_scn;
			if (tp_cnt == 0){
				final_return = RC_RM_NO_MORE_TUPLES;
				return final_return;
			}

			do{
				if(!(sc_cnt<0||scan==0))
				{
					(*scn_Mngr).rcd_ID.slot++;
					if(!((*scn_Mngr).rcd_ID.slot<ttl_slts))
					{
						int tep = 0;
						scn_Mngr->rcd_ID.page++;
						scn_Mngr->rcd_ID.slot = tep;						
					}					
				}
				else
				{
					int td = 0;
					scn_Mngr->rcd_ID.slot = td;
					td++;
					scn_Mngr->rcd_ID.page = td;
				}
				pinPage(&tbl_Mngr->buff_poool, &scn_Mngr->buff_pgs_hdl, (*scn_Mngr).rcd_ID.page);	
				if(val)	
				dat=(*scn_Mngr).buff_pgs_hdl.data;
				dat+=((*scn_Mngr).rcd_ID.slot * getRecordSize(schma));
				if(val)
					(*record).id.slot = scn_Mngr->rcd_ID.slot;
				char *dat_pointer;
				(*record).id.page=scn_Mngr->rcd_ID.page;
				dat_pointer = (*record).data;
				if(val)
				*dat_pointer = '-';
				++dat_pointer;
				memcpy(dat_pointer, dat + 1, getRecordSize(schma) - 1);
				if(val)
				(*scn_Mngr).cnt_scn++;
				++sc_cnt;
				if(val)
					evalExpr(record, schma, (*scn_Mngr).cndt, &rslt); 
				if(val&&(*rslt).v.boolV==TRUE)
				{
					unpinPage(&tbl_Mngr->buff_poool, &scn_Mngr->buff_pgs_hdl);
					final_return = RC_OK;		
					return final_return;
				}
			}
			while(sc_cnt <= tp_cnt);
			unpinPage(&tbl_Mngr->buff_poool,&scn_Mngr->buff_pgs_hdl);
			int er = 0;
			if(val)
				(*scn_Mngr).rcd_ID.slot=er;
			er++;
			(*scn_Mngr).rcd_ID.page=er;
			er--;
			(*scn_Mngr).cnt_scn=er;
		}		
	}
	final_return = RC_RM_NO_MORE_TUPLES;
	return final_return;
}

/**
* closeScan
* ---------
* This function terminates the scanning process. 
* We examine the scanCount value in the table's metadata to determine whether the scan was successful. If the value is greater than zero, the scan was not finished. 
* If the scan fails, we unpin the page and reset all scan mechanism-related variables in our table's meta-data (custom data structure). 
* The metadata space is then made available (de-allocated).
**/

RC closeScan (RM_ScanHandle *scan)
{
	//RC final_return;
	bool val=true;
	if(scan->mgmtData==NULL)
		val=false;
	if(!val){
		final_return = RC_BUFFER_ERROR;
		return final_return;
	}

	if(val){
		Recd_Mngr *scn_Mngr=scan->mgmtData;
		int tp=0;
		Recd_Mngr *rcod_Mngr=(*scan).rel->mgmtData;
		if(scn_Mngr->cnt_scn>tp&&val){
			unpinPage(&rcod_Mngr->buff_poool, &scn_Mngr->buff_pgs_hdl);
			if(val)
				scn_Mngr->rcd_ID.page = 1;
			scn_Mngr->cnt_scn = 0;
			if(scn_Mngr->cnt_scn>0&&val)
				scn_Mngr->rcd_ID.slot = 0;
		}
		scan->mgmtData = NULL;
    	final_return = RC_OK;
    	free(scan->mgmtData);  
	}
	return final_return;
}

/*---------------dealing with schemas-----------------*/

/* As per the assignment requirement, we have 3 functions in dealing with schemas. They are:
int getRecordSize (Schema *schema);
Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys);
RC freeSchema (Schema *schema);
*/

/**
* getRecordSize
* -------------
* This function returns the size of a record in the specified schema. 
* We go through the schema's attributes iteratively. We repeatedly add the size (in bytes) required by each attribute to the variable'size.' 
* The value of the variable'size' represents the size of the record.
**/

int getRecordSize (Schema *schema)
{
	int sz = 0;
	int val = schema->numAttr;
	int cnt = 0; 
	while(cnt<val){
		if(schema->dataTypes[cnt] == DT_BOOL){
		sz += sizeof(bool);
		break;
		}
		if(schema->dataTypes[cnt] == DT_INT){
		sz += sizeof(int);
		break;
		}
		if(schema->dataTypes[cnt] == DT_STRING){
		sz += schema->typeLength[cnt];
		break;
		}
		if(schema->dataTypes[cnt] == DT_FLOAT){
		sz += sizeof(float);
		break;
		}
		cnt += 1;
	}
	sz+=1;
	return sz;
}

/**
* createSchema
* ------------
* With the parameters supplied, this function creates a new schema in memory. 
* numAttr specifies the number of parameters. attrNames specifies the name of the attributes. Datatypes define the datatype of the attributes. typeLength specifies the attribute's length (example: length of STRING). 
* A schema object is created and memory is assigned to it. Finally, we set the parameters of the schema to those specified in the createSchema function (...)
**/

Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
{
	int sz=sizeof(Schema);
	Schema *schma=(Schema*)malloc(sz);
	int tp = 0;
	bool val = true;
	if(keySize<tp||keySize==tp){
		val = false;
		return NULL;
	}
	int temp=1;
	if(val){
	schma->dataTypes=dataTypes;
	schma->keySize=keySize;
	temp = 1;
	}
	if(temp == 1 && val){
	schma->typeLength=typeLength;
	schma->numAttr=numAttr;
	temp=0;
	}
	if(temp == 0 && val){
	schma->attrNames=attrNames;
	schma->keyAttrs=keys;
	temp = 1;
	}
	//free(temp);
	return schma; 
}

/**
* freeSchema
* ----------
* This function clears the memory associated with the schema specified by the'schema' parameter. 
* The variable (field) refNum of each page frame is used for this. refNum keeps track of the number of page frames accessed by the client. 
* Using the C function free(...), we de-allocate the memory space used by the schema, removing it from memory.
**/

RC freeSchema (Schema *schema)
{
	//RC final_return;
	final_return = RC_OK;
	free(schema);
	return final_return;
}

/*---------------dealing with records and attribute values-----------------*/

/* As per the assignment requirement, we have 4 functions in dealing with records and attribute values. They are:
RC createRecord (Record **record, Schema *schema);
RC freeRecord (Record *record);
RC getAttr (Record *record, Schema *schema, int attrNum, Value **value);
RC setAttr (Record *record, Schema *schema, int attrNum, Value *value);
*/

/**
* createRecord
* ------------
* This function creates a new record in the schema specified by the'schema' parameter and passes it to the'record' parameter of the createRecord() function. 
* We ensure that the new record has sufficient memory. We also allocate RAM for the record's data, which is equal to the record's size. 
* We also add '0' to the first position, which is NULL in C, to indicate that this is a new blank record. 
* Finally, we attach this new record to the'record' argument.
**/

RC createRecord (Record **record, Schema *schema)
{
	bool val=true;
	//RC final_return;
	if(val){
		final_return = RC_OK;
		int sz=sizeof(Record);
		Record *rcd = (Record*) malloc(sz);
		int sz1=getRecordSize(schema);
		rcd->data=(char*)malloc(sz1);
		int tp=-1;
		char *dat_pointer;
		if(val)
			rcd->id.page=tp;
		rcd->id.slot=tp;
		dat_pointer=rcd->data;
		if(val)
			*dat_pointer='-';
		if(val)
			*(++dat_pointer)='\0';
		*record=rcd;
		return final_return;
	}
	else{
		final_return = RC_SCHEMA_ERROR;
		return final_return;
	}	
}

/**
* attrOffset
* ----------
* The function's'result' parameter is set to the offset (in bytes) from the initial position to the record's selected attribute. 
* We loop through the attributes of the schema until we reach the specified attribute number. We repeatedly add the size (in bytes) required by each attribute to the pointer *result.
**/

RC attrOffset (Schema *schema, int attrNum, int *result)
{
	//RC final_return;
	int j = 0;
	final_return = RC_OK;
	int tp=0;
	*result = 1;
	int sz;
	bool val = true;
	if(attrNum>=tp&&val){
		block:
		while(true){
		if(schema->dataTypes[j] == DT_BOOL){
		sz = sizeof(bool);
		if(j < attrNum)
			*result=sz+*result;
		break;
		}
		if(schema->dataTypes[j] == DT_INT){
		sz = sizeof(int);
		if(j < attrNum)
			*result=sz+*result;
		break;
		}
		if(schema->dataTypes[j] == DT_FLOAT){
		sz = sizeof(float);
		if(j < attrNum)
			*result=sz+*result;
		break;
		}
		if(schema->dataTypes[j] == DT_STRING){
		if(j < attrNum)
			*result = *result + (*schema).typeLength[j];
		break;
		}
		}			
		++j;
		if(attrNum>j)
			goto block;

	}
	return final_return;
}

/**
* freeRecord
* ----------
* This function de-allocates the memory space allocated to the'record' specified by the parameter. 
* We use the C function free to de-allocate the memory space used by the record ().
**/

RC freeRecord (Record *record)
{
	final_return = RC_OK;
	bool val = false;
	if(true){
		val = true;	
	}
	if(val)
	free(record);

	return final_return;
}

/**
* getAttr
* -------
* This function retrieves an attribute from the given record in the provided schema. 
* The parameter specifies the record, schema, and attribute number for which data is requested. The attribute information is saved to the location specified by the parameter 'value.' 
* To get to the attribute's location, we use the attrOffset(...) function. Depending on the datatype of the attribute, the datatype and value are then copied to the '*value' parameter.
**/

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value)
{
	RC final_return;
	int tp=0;
	int pos=0;
	if(attrNum>=tp){
		char *dat_pointer = (*record).data;
		attrOffset(schema,attrNum,&pos);
		int sz=sizeof(Value);
		Value *attr = (Value*) malloc(sz);
		dat_pointer +=  pos;
		int ed = 1;
		if(pos!=tp){
			if(attrNum != 1){
				schema->dataTypes[attrNum]=(*schema).dataTypes[attrNum];
			}
			else{
				schema->dataTypes[attrNum]=ed;
			}
		}
		bool val = true;
		if(pos!=tp){
			while(val){
			if((*schema).dataTypes[attrNum] == DT_BOOL)
			break;
			if((*schema).dataTypes[attrNum] == DT_INT){
			if(pos!=tp){
						int vle = 0;
						int sze = sizeof(int);
						memcpy(&vle, dat_pointer, sze);
						if(pos!=tp)
							attr->dt = DT_INT;
						attr->v.intV = vle;
					}
					break;
			}
			if((*schema).dataTypes[attrNum] == DT_FLOAT){
			if(pos!=tp){
						float vle;
						int sze = sizeof(float);
						memcpy(&vle, dat_pointer, sze);
						if(pos!=tp)
							attr->dt = DT_FLOAT;
						attr->v.floatV = vle;
					}
					break;
			}
			if((*schema).dataTypes[attrNum] == DT_STRING){
			if(pos != tp){
						int sze = (*schema).typeLength[attrNum];
						sze += 1;
						(*attr).v.stringV = (char *) malloc(sze);
						sze -= 1;
						strncpy((*attr).v.stringV, dat_pointer, sze);
						if(pos != tp)
							(*attr).v.stringV[sze] = '\0';
						(*attr).dt = DT_STRING;
					}
					break;
			}
		}
		}
		if(((*schema).dataTypes[attrNum]) == DT_BOOL){
				bool val;
				int sz2 = sizeof(bool);
				memcpy(&val, dat_pointer, sz2);
				if(pos!=tp)
					(*attr).v.boolV = val;
				(*attr).dt = DT_BOOL;
		}
		final_return = RC_OK;
		*value = attr;
		return final_return;
	}
	else{
		final_return = RC_SCHEMA_ERROR;
		return final_return;
	}	
}

/**
* setAttr
* -------
* This function changes the attribute value in the supplied schema's record. The option takes the record, schema, and attribute number of the data to be retrieved.
* The 'value' option specifies the data to be stored in the attribute.
* Using the attrOffset(...) function, we get to the attribute's location. The data in the '*value' parameter is then copied to the attributes datatype and value, depending on the datatype of the attribute.
**/

RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
{
	//RC final_return;
	int pos = 0;
	final_return = RC_OK;
	int tp = 0;
	char *dat_pointer = (*record).data;
	bool val = true;
	if(attrNum > tp || attrNum >= tp){	
	attrOffset(schema,attrNum,&pos);
			dat_pointer += pos;
			while(val){
			if(schema->dataTypes[attrNum] == DT_BOOL){
			if(attrNum >= tp){
						*(bool*)dat_pointer=(*value).v.boolV;
						int sz = sizeof(bool);
						dat_pointer= sz + dat_pointer;
					}
					break;
			}
			if(schema->dataTypes[attrNum] == DT_INT){
			if(attrNum >= tp){
						*(int *)dat_pointer=(*value).v.intV;
						int sz = sizeof(int);	  
						dat_pointer = sz + dat_pointer;
					}
					break;
			}
			if(schema->dataTypes[attrNum] == DT_FLOAT){
			if(attrNum >= tp){
						*(float *)dat_pointer=(*value).v.floatV;
						int sz = sizeof(float);
						dat_pointer = sz + dat_pointer;
					}
					break;
			}
			if(schema->dataTypes[attrNum] == DT_STRING){
			if(attrNum >= tp){
						strncpy(dat_pointer,(*value).v.stringV,(*schema).typeLength[attrNum]);
						dat_pointer += (*schema).typeLength[attrNum];
					}
					break;
			}
		}
	}
			
	return final_return;
}
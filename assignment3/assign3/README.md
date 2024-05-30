Assignment 3 (CS-525) Record Manager
------------------------------------

Instructions to Run the Code:
----------------------------
On a linux system, open the terminal. Navigate to the assignment directory and then run the following commands shown below:

$ ls                  #to list all the files and check if we are in the correct directory
$ make clean          #to delete old compiled .o files
$ make                #to compile the files   
$ make run            #to run the "test_assign3_1.c" file
$ make test_expr     #to compile test expression files including "test_expr.c"
$ make run_expr       #to run "test_expr.c" file

We designed this record manager with memory management in mind, releasing any reserved 
space and minimizing the use of variables to the greatest extent possible. In addition
to the functions described in record mgr.h, we've implemented the Tombstone mechanism.

There are 5 parts in the assignment. They are:
1. Table and Manager 
2. Handling records in a table
3. Scans 
4. Dealing with Schemas 
5. Dealing with Records and Attribute values 


Table and Manager:

As per the assignment requirement, we have 7 functions in table and manager. They are:
1. initRecordManager (..)
	This function is used to initialize the record manager. To initialize the storage 
manager, we use the Storage Manager's initStorageManager(..) function.

2. shutdownRecordManager(..)
	This function is used to shut down the record manager and de-allocate all of the 
record manager's resources. It frees up all of the Record Manager's resources and memory 
space. We free up memory by setting the recordManager data structure pointer to NULL and 
calling the C function free().

3. createTable(..)
	To make a new table, use this function. It displays the table with the name specified 
by the parameter name. It calls initBufferPool to initialize the Buffer Pool (..). The LRU 
page replacement policy is implemented. It configures the table's attributes 
(name, datatype, and size) as well as all of the table's values. It then creates a page 
file, opens it, writes the table block to the page file, and closes it.

4. openTable(..)
	This function creates a table with the name specified by the parameter 'name' in the 
schema specified by the parameter'schema.' It initializes the buffer pool before pinning a 
page, reading the schema from the file, and unpinning the page.

5. closeTable(..)
	This function is used to close the table specified by the parameter'rel'. It 
accomplishes this by invoking the Buffer Manager function shutdownBufferPool (..). Before 
closing the buffer pool, the buffer manager writes the table changes to the page file.

6. deleteTable(..)
	This function deletes the table with the name specified by the parameter 'name.' It 
invokes the Storage Manager's destroyPageFile function (..). The destroyPageFile(..) function
deletes the page from disk and frees up the memory space reserved for that mechanism. The 
page will not be deleted if destroyPageFile(..) is not RC_OK.

7. getNumTuples(..)
	This function returns the number of tuples in the table specified by the parameter'rel'. 
It returns the value of the variable [tuplesCount] defined in our custom data structure for 
storing table meta-data.

Handling records in a table:
These functions are used to get records with specific RIDs, get records with specific RIDs, 
insert new records, and update existing records with new values.

As per the assignment requirement, we have 4 functions in handling records in a table. They are:
1. insertRecord(..)
	This function inserts a new record into the table and updates the'record' parameter 
with the Record ID passed in by insertRecord(). We assign a Record ID to the record that is 
being inserted. We pin the page with an open slot. When we find an empty slot, we locate the
data pointer and add a '+' to indicate that a new record has been added. After inserting the
record, we mark the page dirty so that the Buffer Manager can write the page's content back 
to disk. We use the memcpy() C function to copy the record's data (passed through parameter'record') 
into the new record, and then the page is unpinned.

2. deleteRecord(..)
	This function is used to delete a record having Record ID 'id' passed through the 
parameter from the table referenced by the parameter 'rel'. We set the freePage meta-data 
in our table to the Page ID of the page whose record is to be deleted, so that this space 
can subsequently be used by a new record. We pin the page and browse to the record's data 
pointer, where we change the first character to '-,' indicating that the record has been 
removed and is no longer required. Finally, we unpin the page and designate it dirty so 
that the Buffer Manager can store the page's contents back to disk.

3. updateRecord(..)
	This function updates a record in the table specified by the parameter "record," which 
is referred to by the argument "rel." It uses the meta-data of the table to find the page 
where the record is located and pins that page to the buffer pool. It assigns a Record ID 
and navigates to the data storage location for the record. We use the memcpy() C function 
to copy the data from the record (provided via parameter'record') into the new record, mark 
the page dirty, and then unpin the page.

4. getRecord(..)
	This function retrieves a record from the table specified by "rel," which is also 
specified in the argument, using the Record ID "id" as a parameter. The recorded data is 
saved in the location specified by the "record" option. It locates the record's page using 
the table's meta-data and then pins that page in the buffer pool using the record's 'id'. 
It sends the data and sets the Record ID of the'record' parameter to the id of the record 
on the page. After that, the page is unpinned.

Scans:
The Scan functions are used to find all tuples in a table that meet a set of criteria 
(represented as an Expr). When a scan is started, the RM_ScanHandle_data structure provided 
as an input to startScan is initialized. Following that, calls to the next method are made, 
which returns the next tuple that matches the scan condition. The result is RC_SCAN_CONDITION_NOT_FOUND 
if the scan condition is NULL. When the scan is complete, RC RM NO MORE TUPLES is returned; 
otherwise, RC_OK is returned (unless an error occurs).

As per the assignment requirement, we have 3 functions in scans. They are:
1. startScan(..)
	The startScan() function initiates a scan by reading data from the RM ScanHandle data 
structure, which is passed as an argument. We create the scan-related variables in our 
one-of-a-kind data structure. If the condition is NULL, the error code RC_SCAN_CONDITION_NOT_FOUND 
is returned.

2. next(..)
	This function returns the next tuple that meets the condition (test expression). If 
condition is NULL, we return the error code RC_SCAN_CONDITION_NOT_FOUND. If there are no 
tuples in the table, the error code RC RM_NO_MORE_TUPLES is returned. We loop through the 
tuples in the table. To evaluate the test expression, pin the page with that tuple, navigate 
to the data location, copy data into a temporary buffer, and then use eval (..). If the 
result of the test expression (v.boolV) is TRUE, the tuple meets the requirement. 
Following that, the page is unpinned, and RC_OK is returned. The error code 
RC_RM_NO_MORE_TUPLES is returned if none of the tuples satisfy the criterion.

3. closeScan(..) 
	This function terminates the scanning process. We examine the cnt_scn value in the 
table's metadata to determine whether the scan was successful. If the value is greater than 
zero, the scan was not finished. If the scan fails, we unpin the page and reset all scan 
mechanism-related variables in our table's meta-data (custom data structure). The metadata 
space is then made available (de-allocated).

Dealing with Schemas:
These functions are used to return the number of records for a given schema in bytes and to 
create a new schema.

As per the assignment requirement, we have 3 functions in dealing with schemas. They are:
1. getRecordSize(..)
	This function returns the size of a record in the specified schema. We go through the 
schema's attributes iteratively. We repeatedly add the size (in bytes) required by each 
attribute to the variable'size.' The value of the variable'size' represents the size of the 
record.

2. freeSchema(..)
	This function clears the memory associated with the schema specified by the'schema' 
parameter. The variable (field) refNum of each page frame is used for this. refNum keeps 
track of the number of page frames accessed by the client. Using the C function free(..), 
we de-allocate the memory space used by the schema, removing it from memory.

3. createSchema(..)
	With the parameters supplied, this function creates a new schema in memory. numAttr 
specifies the number of parameters. attrNames specifies the name of the attributes. 
Datatypes define the datatype of the attributes. typeLength specifies the attribute's 
length (example: length of STRING). A schema object is created and memory is assigned to it. 
Finally, we set the parameters of the schema to those specified in the createSchema function.

Dealing with Records and Attribute values:
These functions are used to get or set the values of a record's attributes, as well as to 
create a new record for a particular schema. When creating a new record, ensure that the 
data field has enough memory to store the binary representations for all of the record's 
characteristics, as specified by the schema.

1. createRecord(..)
	This function creates a new record in the schema specified by the'schema' parameter 
and passes it to the'record' parameter of the createRecord() function. We ensure that the 
new record has sufficient memory. We also allocate RAM for the record's data, which is 
equal to the record's size. We also add '0' to the first position, which is NULL in C, to 
indicate that this is a new blank record. Finally, we attach this new record to the'record' 
argument.

2. attrOffset(..)
	The function's'result' parameter is set to the offset (in bytes) from the initial 
position to the record's selected attribute. We loop through the attributes of the schema 
until we reach the specified attribute number. We repeatedly add the size (in bytes) required 
by each attribute to the pointer *result.

3. freeRecord(..)
	This function de-allocates the memory space allocated to the'record' specified by the 
parameter. We use the C function free to de-allocate the memory space used by the record ().

4. getAttr(..)
	This function retrieves an attribute from the given record in the provided schema. The 
parameter specifies the record, schema, and attribute number for which data is requested. 
The attribute information is saved to the location specified by the parameter 'value.' To get 
to the attribute's location, we use the attrOffset(...) function. Depending on the datatype 
of the attribute, the datatype and value are then copied to the '*value' parameter.

5. setAttr(..)
	This function changes the attribute value in the supplied schema's record. The option 
takes the record, schema, and attribute number of the data to be retrieved. The 'value' 
option specifies the data to be stored in the attribute. Using the attrOffset(...) function,
we get to the attribute's location. The data in the '*value' parameter is then copied to 
the attributes datatype and value, depending on the datatype of the attribute.
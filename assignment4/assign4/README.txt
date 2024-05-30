CS 525 = Assignment-4: B+-TREE
=======================================

RUNNING THE SCRIPT
==================

1) Go to Project root (assign4) using Terminal.
2) Type ls to list the files and check that we are in the correct directory.
3) Type "make clean" to delete old compiled .o files.
4) Type "make" to compile all project files including "test_assign4_1.c" file 
5) Type "make run" to run "test_assign4_1.c" file.


SOLUTION DESCRIPTION
====================



1) INITIALIZE AND SHUTDOWN INDEX MANAGER
========================================

These functions are used to initialize and shut down the index manager, releasing all acquired resources.

initIndexManager()
==================
1) This function initializes the index manager.
2) This function is called to initialize the storage manager. 

shutdownIndexManager()
======================
1) This function shuts down the index manager and de-allocates all index manager allocated resources.
2) It free up all memory being used by the Index Manager.


2) B+ TREE INDEX RELATED FUNCTIONS
==================================

These functions are used to create, open, close and delete a b-tree index.

createBtree()
=============
1) This function creates a new B+ Tree.
2) It creates the TreeManager structure, which contains extra information about our B+ Tree.
3) We initialize the buffer manager, construct a buffer pool with Buffer Manager, then create a page with the supplied page name "idxId" with Storage Manager.

openBtree()
===========
1) This function opens an existing B+ Tree that is stored on the file specified by "idxId" parameter.

closeBtree()
============
1) This function closes the B+ Tree.
2) This function flags all pages dirty so that the Buffer Manager can write them back to disk.
3) It then terminates the buffer pool and releases all assigned resources.

deleteBtree()
=============
1) This function deletes the page file with the file name "idxId" in the parameter. This is accomplished through the usage of Storage Manager.


3) FUNCTIONS WITH ACCESS INFORMATION 
====================================

These functions assist us in obtaining information about our B+ Tree, such as the number of nodes and keys in our tree.

getNumNodes()
=============
1) This function returns the number of nodes present in our B+ Tree.
2) We store this information in our BTreeRecordMgr structure in "number_of_nodes" variable. 

getNumEntries()
===============
1) This function returns the number of entries or records or keys present in our B+ Tree.
2) We store this information in our BTreeRecordMgr structure in "number_of_node_entries" variable. 

getKeyType()
============ 
1) This function returns the datatype of the keys being stored in our B+ Tree.
2) We store this information in our BTreeRecordMgr structure in "type_of_key" variable.


4) ACCESSING B+ TREE FUNCTIONS
==============================

1) These functions are used to find, insert, and delete keys in/from a given B+ Tree. 
2) Also, we can scan through all entries of a B+ Tree in sorted order using the openTreeScan, nextEntry, and closeTreeScan methods.

findKey()
=========
1) This function searches the B+ Tree for the key specified in the parameter.
2) If an entry with the specified key is found, we store the RID (value) for that key in the memory location pointed by "result" parameter.
3) We call findRecord() function which serves the purpose. If findRecord() returns NULL, it means the key is not present in B+ Tree and we return error code RC_IM_KEY_NOT_FOUND.

insertKey()
===========
1) This function inserts a new entry/record with the specified key and RID.
2) We first, search the B+ Tree for the specified key. If it is found, then we return error code RC_IM_KEY_ALREADY_EXISTS.
3) If it is not found, then we create a TreeNodeData structure which stores the RID.
4)Then, We check if root of the tree is empty. If it's empty, then we call createNewTree() which creates a new B+ Tree and adds this entry to the tree.
5)Else if our tree has a root element i.e. tree was already there, then we locate the leaf node where this key can be inserted.
6) Once the leaf node has been found, then we check if it has space for the new entry. If yes, then we call insertRecordIntoLeaf() which performs the insertion.
7) If the leaf node is full, the we call insertValueIntoLeafAfterSplitting() which splits the leaf node and then inserts the entry.

deleteKey()
===========
1) This function deletes the record with the specified "key" in the B+ Tree.
2) We call our B+ Tree method deleteTreeRecord() as explained above. 
3) This function deletes the key from the tree and adjusts the tree accordingly so as to maintain the B+ Tree properties.

openTreeScan()
==============
1) This function initializes the scan which is used to scan the entries in the B+ Tree in the sorted key order.
2) This function initializes our BtreeScanMgr structure which stores extra information for performing the scan operation. 
3) If the root node of the B+ Tree is NULL, then we return error code RC_NO_RECORDS_TO_SCAN.

nextEntry()
===========
1) This function is used to traverse the entries in the B+ Tree.
2) It stores the record details.
3) If all the entries have been scanned and there are no more entries left, then we return error code RC_IM_NO_MORE_ENTRIES;

closeTreeScan()
===============
1) This function closes the scan mechanism and frees up resources.


5) DEBUGGING AND TEST FUNCTIONS
=========================================

These functions are used for debugging purpose.  

printTree()
===========
1) This function prints the B+ Tree.


6) CUSTOM B+ TREE FUNCTIONS 
===========================
These functions have been defined to perform insert/delete/find/print operations on our B+ Tree.

leafFinder()
==========
1) This function finds the leaf node containing the entry having the specified key in parameter.
2) It is used when inserting an element as well as finding an entry.

recordFinder()
============
1) This function searches our B+ Tree for an entry having the specified key in parameter.
2) It returns the record if the key is present in the tree else returns null.

createNewRecord()
============
1) This function creates a new record element which encapsulates a RID.

insertRecordIntoLeaf()
================
1) This function inserts a new pointer to the record and its corresponding key into a leaf.
2) It returns the altered leaf node.

createNewBTree()
===============
1) This function creates a new tree when the first element is inserted in the B+ tree.

createNewNode()
============
1) This function creates a new general node, which can be adapted to serve as a leaf/internal/root node.

createNewLeaf()
============
1) This function creates a new leaf node.

insertValueIntoLeafAfterSplitting()
==============================
1) This function inserts a new key and pointer to a new record into a leaf node so as to exceed the tree's order, causing the leaf to be split in half.
2) It adjusts the tree after splitting so as to maintain the B+ Tree properties.

insertValueIntoNode()
================
1) This function inserts a new key and pointer to a node into a node into which these can fit without violating the B+ tree properties.

insertValueIntoNonLeafNodeAfterSplitting()
==============================
1) This function inserts a new key and pointer to a node into a non-leaf node, causing the node's size to exceed the order, and causing the node to split into two.

insertValueIntoParentNode()
==================
1) This function inserts a new node (leaf or internal node) into the B+ tree.
2) It returns the root of the tree after insertion.

insertValueIntoNewRoot()
===================
1) This function creates a new root for two subtrees and inserts the appropriate key into the new root.

getLeftIndexValue()
==============
1) This function is used in insertValueIntoParentNode() function to findRecord the index of the parent's pointer to the node to the left of the key to be inserted.

adjustingRoot()
============
1) This function adjusts the root after a record has been deleted from the B+ Tree and maintains the B+ Tree properties.

mergeNodesAfterDeletion()
============
1) This function combines (merges) a node that has become too small after deletion with a neighboring node that can accept the additional entries without exceeding the maximum.

redistributeNodesAfterDeletion()
===================
1) This function redistributes the entries between two nodes when one has become too small after deletion but its neighbor is too big to append the small node's entries without exceeding the maximum.

deleteRecordEntry()
=============
1) This function deletes an entry from the B+ tree.
2) It removes the record having the specified key and pointer from the leaf, and then makes all appropriate changes to preserve the B+ tree properties.

deleteTreeRecord()
========
1) This function deletes the the entry/record having the specified key.

removeRecordEntryFromNode()
=====================
1) This function removes a record having the specified key from the the specified node.

getNeighborNodeIndex()
==================
1) This function returns the index of a node's nearest neighbor (sibling) to the left if one exists.
2) It returns -1 if this is not the case.
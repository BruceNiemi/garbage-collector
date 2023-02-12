#include <stdio.h>
#include <string.h>
#include "ObjectManager.h"

static int testsExecuted = 0;
static int testsFailed = 0;

//------------------------------------------------------
// testInitalizePool
//
// PURPOSE: Testing initalizing the object pool
//------------------------------------------------------
void testInitalizePool()
{
    testsExecuted++;
    fprintf(stderr, "Testing initalizing the object pool\n\n");

    initPool();

    fprintf(stderr, "Dumping pool nothing should be in this pool.\n\n");
    dumpPool(); // verify nothing exists in the a

    fprintf(stderr, "Destorying.\n\n");
    destroyPool(); // destory

    fprintf(stderr, "Testing some edge cases\n\n");

    fprintf(stderr, "Testing double initlization of the pool.\n");
    initPool();
    initPool();
    destroyPool();

    fprintf(stderr, "Testing double destruction of the pool.\n");

    initPool();
    destroyPool();
    destroyPool();
}

//------------------------------------------------------
// testInsertEmpty
//
// PURPOSE: Testing insertion to an empty object pool.
//------------------------------------------------------
void testInsertEmpty()
{
    testsExecuted++;
    fprintf(stderr, "Testing insertion to an empty object pool.\n");
    initPool();
    
    Ref first = insertObject(100);

    if (first == 1)
    {
        fprintf(stderr, "SUCCESS: Inserted 100 bytes into an object pool\n");
    }
    else
    {
        testsFailed++;
        fprintf(stderr, "ERROR: Failed to insert 100 bytes into an object pool\n");
    }

    destroyPool();
}

//------------------------------------------------------
// testCompaction
//
// PURPOSE: Testing insertion and compaction.
//------------------------------------------------------
void testCompaction()
{
    testsExecuted++;
    fprintf(stderr, "\nTesting insertion and compaction.\n");
    initPool();
    
    insertObject(100);
    insertObject(1000);
    Ref compact = insertObject(10000);

    dropReference(compact);  // drop this

    insertObject(MEMORY_SIZE); 

    if (retrieveObject(compact) == NULL_REF)
    {
        fprintf(stderr, "SUCESS: Compacted '10000' bytes\n");
    }
    else
    {
        testsFailed++;
        fprintf(stderr, "ERROR: Failed compact object manager.\n");
    }

    
    destroyPool();
}

//------------------------------------------------------
// testInsertAndRetrive
//
// PURPOSE: Testing inserting data into the pool and filling the allocated buffer.
//------------------------------------------------------
void testInsertAndRetrive()
{
    testsExecuted++;
    fprintf(stderr, "\nTesting inserting data into the pool and filling the allocated buffer.\n");
    initPool();

    char a_array[100] = {};
    for (size_t i = 0; i <100; i++)
    {
        a_array[i] = 'a';
    }
    
    
    Ref a = insertObject(100);


    char* array_a = NULL_REF;

    array_a = (char*) retrieveObject(a);

    if (array_a != NULL_REF)
    {
        for (size_t i = 0; i < 100; i++)
        {
            array_a[i] = 'a';
        }
    }
    else
    {
        testsFailed++;
        fprintf(stderr, "ERROR: Failed allocated '100' bytes of memory and insert.\n");
        return;
    }

    
    char* array_b = NULL_REF;

    array_b = (char*) retrieveObject(a);


    if (array_b != NULL_REF && strncmp(array_b, a_array, 100) == 0)
    {
        fprintf(stderr,"SUCESS: Sucessfully allocated and inserted '100' bytes of memory.\n");
    }
    else
    {
        testsFailed++;
        fprintf(stderr, "ERROR: Failed allocated '100' bytes of memory and insert.\n");
    }
    

    destroyPool();
}

//------------------------------------------------------
// testInsertFull
//
// PURPOSE: Testing inserting data into a pool with maximum capacity.
//------------------------------------------------------
void testInsertFull()
{
    testsExecuted++;
    fprintf(stderr, "\nTesting inserting data into a pool with maximum capacity.\n");
    initPool();

    for (size_t i = 0; i < 512; i++)
    {
        insertObject(1024);
    }
    
    Ref ref =  insertObject(1024);

    if (ref == NULL_REF)
    {
        fprintf(stderr, "SUCESS: Didn't insert '1024' bytes into full array.\n");
    }
    else
    {
        testsFailed++;
        fprintf(stderr, "FAILED: Inserted '1024' bytes into full array.\n");
    }
    

    destroyPool();
}

//------------------------------------------------------
// testRevialEmpty
//
// PURPOSE: Testing retevial in an empty object pool.
//------------------------------------------------------
void testRevialEmpty()
{
    testsExecuted++;
    fprintf(stderr, "\nTesting retevial in an empty object pool.\n");
    initPool();

    void* ref = (void*)retrieveObject(10);

    if (ref == NULL_REF)
    {
        fprintf(stderr, "SUCESS: Couldn't find reference that is not inserted into the pool.\n");
    }
    else
    {
        testsFailed++;
        fprintf(stderr, "FAILED: Found reference that is not inserted into the pool.\n");
    }
    

    destroyPool();    
}

//------------------------------------------------------
// testAddReference
//
// PURPOSE: Testing increasing reference count
//------------------------------------------------------
void testAddReference()
{
    testsExecuted++;
    fprintf(stderr, "\nTesting increasing reference count.\n");
    initPool();

    Ref ref  = insertObject(100);

    if (ref == NULL_REF)
    {
        testsFailed++;
        fprintf(stderr, "FAILED: Could not insert into pool when testing incrementing reference count.\n");
    }
    else
    {
        addReference(ref);

        fprintf(stderr, "INFO: There should be '2' counts for the reference with id of '1'.\n");
        dumpPool();
    }

    destroyPool();    
}

//------------------------------------------------------
// testDropReference
//
// PURPOSE: Testing drop reference count.
//------------------------------------------------------
void testDropReference()
{
    testsExecuted++;
    fprintf(stderr, "\nTesting drop reference count.\n");
    initPool();

    Ref ref  = insertObject(100);

    if (ref == NULL_REF)
    {
        testsFailed++;
        fprintf(stderr, "FAILED: Could not insert into pool when testing dropping reference count.\n");
    }
    else
    {
        dropReference(ref);

        fprintf(stderr, "INFO: There should be '0' counts for the reference with id of '1'.\n");
        dumpPool();
    }

    destroyPool();    
}


int main(int argc, char const *argv[])
{
    fprintf(stderr, "------------------------------------------------\n");
    testInitalizePool();
    fprintf(stderr, "------------------------------------------------\n");
    testInsertEmpty();
    fprintf(stderr, "------------------------------------------------\n");
    testCompaction();
    fprintf(stderr, "------------------------------------------------\n");
    testInsertAndRetrive();
    fprintf(stderr, "------------------------------------------------\n");
    testInsertFull();
    fprintf(stderr, "------------------------------------------------\n");
    testRevialEmpty();
    fprintf(stderr, "------------------------------------------------\n");
    testAddReference();
    fprintf(stderr, "------------------------------------------------\n");
    testDropReference();

    printf("\nTotal number of tests executed: %d\n", testsExecuted);
    printf("Number of tests passed:         %d\n",
         (testsExecuted - testsFailed));
    printf("Number of tests failed:         %d\n", testsFailed);
    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "ObjectManager.h"

//-------------------------------------------------------------------------------------
// CONSTANTS and TYPES
//-------------------------------------------------------------------------------------

struct REFERENCE
{
    int size;
    int address;
    Ref id;
    int count;
};
typedef struct REFERENCE Reference;

typedef struct NODE Node;

struct NODE
{
    Reference ref;
    Node* next;
};


//------------------------------------------------------
// createReference
//
// PURPOSE: Creates a new references to be inserted into our linked list.
// INPUT PARAMETERS:
// size - The size of the object that is being inserted into the list
// address - The offset in the buffer were the object lives
// id - The unique id of the object
// count - The current count of the object.
// object pool.
// OUTPUT PARAMETERS:
// A new reference to be insted into the list
//------------------------------------------------------
static Reference createReference(const int size, const int address, const Ref id, const int count)
{
    Reference reference1;
    reference1.size = size;
    reference1.address = address;
    reference1.id  = id;
    reference1.count = count;
    return reference1;
}

//------------------------------------------------------
// createNode
//
// PURPOSE: Creates a new node to be placed within our linked list
// INPUT PARAMETERS:
// ref - The reference that is attached to the currnet object
// next - The next node in the linked list
// OUTPUT PARAMETERS:
// Either a new node or NULL.
//------------------------------------------------------
static Node* createNode(const Reference ref, Node *next)
{
    // this is an edge case with clang++ were we must cast to the type
    Node* node = NULL;

    node = (Node*) malloc(sizeof(Node));

    assert(node != NULL);

    if (node != NULL_REF)
    {
        node->ref = ref;
        node->next = next;
    }
    else
    {
        fprintf(stdout, "Failed to dynamically allocate memory for new node\n");
    }

    return node;
}

//-------------------------------------------------------------------------------------
// VARIABLES
//-------------------------------------------------------------------------------------

// used for id generation
static unsigned long references = 0;

// We will use double buffering
unsigned char* buffer;

// free ptr location
int freePtr = 0;

// linked list
Node *top;

// object
unsigned long totalObjects = 0;

//-------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------

//------------------------------------------------------
// verifyState
//
// PURPOSE: Verifies the state of the object pool to enusre
// that it's valid when modifying values upon it.
//------------------------------------------------------
static void verifyState(void)
{
    assert(buffer != NULL);
}

//------------------------------------------------------
// compact
//
// PURPOSE: Runs the mark and compact algorithm on the object pool
//------------------------------------------------------
static void compact(void)
{
    verifyState();
    unsigned char* temp = NULL_REF;

    int newFreePtr = 0;
    int bytesUsed = 0;
    int bytesCollected = 0;

    Node* newTop = NULL_REF;

    temp = (unsigned char*)  malloc(MEMORY_SIZE * sizeof(unsigned char));

    assert(temp != NULL_REF);

    if (temp != NULL_REF)
    {
        Node* current = top;

        while (current != NULL_REF)
        {
            assert(current != NULL_REF);

            bytesUsed += current->ref.size;

            if (current->ref.count > 0)
            {
                memcpy(&temp[newFreePtr], &buffer[current->ref.address], current->ref.size);

                current->ref.address = newFreePtr;
                newFreePtr += current->ref.size;

                newTop = createNode(current->ref, newTop);
                assert(newTop != NULL_REF);
            }
            else
            {
                bytesCollected += current->ref.size;
            }
            
            current = current->next; 
        }

        current = top;

        Node* tempNode = NULL_REF;

        while (current != NULL_REF)
        {
            assert(current != NULL_REF);
            
            tempNode = current->next;

            free(current);

            current = tempNode;
        }

        free(buffer); 
        buffer = temp;
        freePtr = newFreePtr;
        top = newTop;
    }
    else
    {
        fprintf(stdout, "Failed to dynamically allocate buffer for compation\n");
    }

    verifyState();
    
    fprintf(stdout, "\nGARBAGE COLLECTION STATS\n");
    fprintf(stdout, "-----------------------\n");
    fprintf(stdout, "Total objects: %lu\n", totalObjects);
    fprintf(stdout, "Total number of bytes used: %d\n", bytesUsed);
    fprintf(stdout, "Total number of bytes collected: %d\n", bytesCollected);
    fprintf(stdout, "-----------------------\n");
}

//------------------------------------------------------
// insertOjbect
//
// PURPOSE: Attemps to insert an object into the pool
// INPUT PARAMETERS:
// size - The size of the object that is being inserted into the
// object pool.
// OUTPUT PARAMETERS:
// Either the reference that was inserted into the pool or NULL_REF
// if we could not allocate memory for the object.
//------------------------------------------------------
Ref insertObject( const int size )
{
    verifyState();
    Ref id = NULL_REF;

    assert(size >= 0);

    if (size >= 0)
    {
        assert(size <= MEMORY_SIZE);

        if (size <= MEMORY_SIZE)
        {
            // Try to compact the pool
            if((freePtr + size) >= MEMORY_SIZE)
            {
                compact();
            }
            
            if ((freePtr + size) <= MEMORY_SIZE)
            {
                Reference ref = createReference(size, freePtr, (++references), 1);

                // Now insert the Reference to the top of the linked list.
                Node* node = createNode(ref, top);

                if (node != NULL_REF)
                {
                    totalObjects++;

                    freePtr += size;

                    top = node;

                    id = ref.id;
                }
                else
                {
                    references--;
                    fprintf(stdout, "Could not insert node into top of linked list\n");
                }

                assert(top != NULL_REF);
            }
            else
            {
                fprintf(stdout, "After compaction object pool is full cannot insert object of size %d.\n", size);
            }
        }
        else
        {
            fprintf(stdout, "object exceddes memory size.\n");
        }
    }
    else
    {
        fprintf(stdout, "size of inserted object is negative and is required to be k >= 0.\n");
    }

    return id;
}

//------------------------------------------------------
// retrieveObject
//
// PURPOSE: Looks up an object in our pool to return the
// address of the pointer.
// INPUT PARAMETERS:
// ref - The id we are looking for to find the value in.
// OUTPUT PARAMETERS:
// Either a pointer to the object or NULL_REF
//------------------------------------------------------
void *retrieveObject( const Ref ref )
{
    verifyState();
    unsigned char * object = NULL_REF;

    assert(ref > 0);

    if (ref > 0)
    {
        Node* current = top;
            
        while (current != NULL_REF && current->ref.id != ref)
        {
            current = current->next;
        }
        
        if (current != NULL_REF)
        {
            assert(current->ref.address >= 0 && current->ref.address < MEMORY_SIZE);
        
            assert(buffer != NULL_REF);

            if (buffer != NULL_REF && current->ref.address >= 0 && current->ref.address < MEMORY_SIZE)
            {
                object = (unsigned char *) &buffer[current->ref.address];

                assert(object != NULL_REF);
            }
            else
            {
                fprintf(stdout, "Current buffer is null. Is the object pool initalized\n");
            }
        }
        else
        {
            fprintf(stdout, "Could not find reference with id of '%lu'.\n", ref);
        }
    }
    else
    {
        fprintf(stdout, "Reference id must be greater than zero current '%lu'.\n", ref);
    }   

    return object;
}

//------------------------------------------------------
// addReference
//
// PURPOSE: Increments a refernce that is in the pool
// INPUT PARAMETERS:
// ref the refernce that is being incremented
//------------------------------------------------------
void addReference( const Ref ref )
{
    verifyState();

    assert(ref > 0);

    if (ref > 0)
    {
        Node* current = top;
            
        while (current != NULL_REF && current->ref.id != ref)
        {
            current = current->next;
        }
        
        assert(current != NULL_REF);

        if (current != NULL_REF)
        {
            current->ref.count++;
        }
        else
        {
            fprintf(stdout, "Could not find reference with id of '%lu'.\n", ref);
        }
    }
    else
    {
        fprintf(stdout, "Reference id must be greater than zero current '%lu'.\n", ref);
    }    
}

//------------------------------------------------------
// dropReference
//
// PURPOSE: Reduces the amount of refernces to an object in the pool
// used during the compaction to free the memory. 
// INPUT PARAMETERS:
// ref - The refece that is being decremented from the pool
//------------------------------------------------------
void dropReference( const Ref ref )
{
    verifyState();

    assert(ref > 0);

    if (ref > 0)
    {
        Node* current = top;

        while (current != NULL_REF && current->ref.id != ref)
        {
            current = current->next;
        }

        assert(current != NULL_REF);
        
        if (current != NULL_REF)
        {
            current->ref.count--;
        }
        else
        {
            fprintf(stdout, "Could not find reference with id of '%lu'.\n", ref);
        }
    }
    else
    {
        fprintf(stdout, "Reference id must be greater than zero current '%lu'.\n", ref);
    }   
}

//------------------------------------------------------
// myRoutine
//
// PURPOSE: Initalizes the object pool with the assigned amount of memory.
//------------------------------------------------------
void initPool() 
{
    // this is an edge case with clang++ were we must cast to the type
    buffer = (unsigned char *)  malloc(MEMORY_SIZE * sizeof(unsigned char));

    assert(buffer != NULL_REF);

    if (buffer == NULL_REF)
    {
        fprintf(stdout, "Failed to dynnamically allocate memory for object pool.\n");
    }
}

//------------------------------------------------------
// destroyPool()
//
// PURPOSE: Removes all information that is stored with-in the pool.
//------------------------------------------------------
void destroyPool()
{
    // To clean up we must reference the linked list and the two buffers.
    Node* current = top;
    Node* temp = NULL_REF;

    while (current != NULL_REF)
    {
        temp = current->next;

        free(current);

        current = temp;
    }

    if(buffer != NULL_REF) free(buffer);

    freePtr = 0;
    references = 0;
    totalObjects = 0;

    buffer = NULL_REF;
    top = NULL_REF;

    assert(buffer == NULL);
    assert(top == NULL);
    assert(freePtr == 0);
    assert(references == 0);
    assert(totalObjects == 0);
}

//------------------------------------------------------
// dumpPool
//
// PURPOSE: Used for debugging the object manager by printing information about the pool
//------------------------------------------------------
void dumpPool()
{
    fprintf(stdout, "\nOBJECT POOL DUMP\n");
    fprintf(stdout, "-----------------------\n");
    if (top == NULL_REF)
    {
        fprintf(stdout, "Empty object pool\n");
    }
    else
    {
        Node* current = top;

        while (current != NULL_REF)
        {
            // we store size in bytes
            // not sure if the address is right
            fprintf(stdout, "Reference(id=%lu, address=%d, size=%d, count=%d)\n", current->ref.id, current->ref.address, current->ref.size, current->ref.count);

            current = current->next;
        }
    }
    fprintf(stdout, "-----------------------\n");
}
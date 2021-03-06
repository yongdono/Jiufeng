/**
 *  @file jf_linklist.c
 *
 *  @brief Implementation file for linked list data structure.
 *
 *  @author Min Zhang
 *  
 *  @note
 *
 */

/* --- standard C lib header files -------------------------------------------------------------- */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#if defined(WINDOWS)

#elif defined(LINUX)
    #include <unistd.h>
#endif

/* --- internal header files -------------------------------------------------------------------- */
#include "jf_basic.h"
#include "jf_limit.h"
#include "jf_err.h"
#include "jf_linklist.h"
#include "jf_jiukun.h"

/* --- private data/data structure section ------------------------------------------------------ */



/* --- private routine section ------------------------------------------------------------------ */



/* --- public routine section ------------------------------------------------------------------- */

void jf_linklist_init(jf_linklist_t * pList)
{
    assert(pList != NULL);

    pList->jl_pjlnHead = NULL;
}

/** Finalize the linked list
 *
 */
void jf_linklist_fini(jf_linklist_t * pList)
{
    jf_linklist_node_t * pjln, * pNode;

    assert(pList != NULL);

	pjln = pList->jl_pjlnHead;
    while (pjln != NULL)
    {
		pNode = pjln->jln_pjlnNext;
        jf_jiukun_freeMemory((void **)&pjln);
        pjln = pNode;
    }

    jf_linklist_init(pList);
}

/** Finalize the linked list with the function to free the data
 * 
 */
void jf_linklist_finiListAndData(
    jf_linklist_t * pList, jf_linklist_fnFreeNodeData_t fnFreeData)
{
    jf_linklist_node_t * pjln, * pNode;

    assert((pList != NULL) && (fnFreeData != NULL));

	pjln = pList->jl_pjlnHead;
    while (pjln != NULL)
    {
		pNode = pjln->jln_pjlnNext;

		fnFreeData(&(pjln->jln_pData));

        jf_jiukun_freeMemory((void **)&pjln);
        pjln = pNode;
    }

    jf_linklist_init(pList);
}

/** Append the data to the linked list
 *
 */
u32 jf_linklist_appendTo(jf_linklist_t * pList, void * pData)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    jf_linklist_node_t * pNode = NULL, * pjln = NULL;

    assert(pList != NULL);

    u32Ret = jf_jiukun_allocMemory((void **)&pNode, sizeof(*pNode));
    if (u32Ret == JF_ERR_NO_ERROR)
    {
        ol_bzero(pNode, sizeof(*pNode));
		pNode->jln_pData = pData;

		if (pList->jl_pjlnHead == NULL)
        {
            pList->jl_pjlnHead = pNode;
        }
        else
        {
			pjln = pList->jl_pjlnHead;
			while (pjln->jln_pjlnNext != NULL)
				pjln = pjln->jln_pjlnNext;

			pjln->jln_pjlnNext = pNode;
        }
    }

    return u32Ret;
}

/** Insert the data to the head of the linked list
 *
 */
u32 jf_linklist_insertTo(jf_linklist_t * pList, void * pData)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    jf_linklist_node_t * pNode;

    assert((pList != NULL) && (pData != NULL));

    u32Ret = jf_jiukun_allocMemory((void **)&pNode, sizeof(*pNode));
    if (u32Ret == JF_ERR_NO_ERROR)
    {
		pNode->jln_pData = pData;

		pNode->jln_pjlnNext = pList->jl_pjlnHead;
		pList->jl_pjlnHead = pNode;
    }

    return u32Ret;
}

u32 jf_linklist_iterate(jf_linklist_t * pList, jf_linklist_fnOpNode_t fnOpNode, void * pArg)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    jf_linklist_node_t * pjln = NULL, * pNode = NULL;

    assert(pList != NULL);

	pjln = pList->jl_pjlnHead;
    while ((pjln != NULL) && (u32Ret == JF_ERR_NO_ERROR))
    {
		pNode = pjln->jln_pjlnNext;

        u32Ret = fnOpNode(pjln, pArg);
        if (u32Ret == JF_ERR_NO_ERROR)
            pjln = pNode;
    }

    return u32Ret;
}

/*------------------------------------------------------------------------------------------------*/


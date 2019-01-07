//Create_a_List.h
#ifndef CREATE_A_LIST_H
#define CREATE_A_LIST_H
#include <WinSock2.h>
#include <Windows.h>
#include <malloc.h>
#include <string.h>
#pragma comment(lib,"ws2_32.lib")
typedef struct Client_Data {
	int Pos;
	SOCKET Socket;
	struct Client_Data *Next;
}CLIENT_DATA;
typedef struct ListHead{
	CLIENT_DATA *DataHead;
}LIST_HEAD;
template <typename NODEHEAD>
NODEHEAD *CreateNode(const NODEHEAD *ptrNode);
template <typename NODEHEAD>
int GetNodeCount(const NODEHEAD *ListHead);
template <typename NODEHEAD>
NODEHEAD *GetNodeAddress(NODEHEAD *ListHead, const NODEHEAD *FindType, int Index, BOOL Last);
template <typename NODEHEAD>
NODEHEAD *InsertNode(NODEHEAD **ListHead, const NODEHEAD *ptrNode, int Index);
template <typename NODEHEAD>
BOOL DeleteNode(NODEHEAD **ListHead, const NODEHEAD *ptrNode, int Index);
template <typename NODEHEAD>
BOOL DestroyList(NODEHEAD **ListHead);

template <typename NODEHEAD>
NODEHEAD *CreateNode(const NODEHEAD *ptrNode){
	NODEHEAD *ptrNew = (NODEHEAD *)malloc(sizeof(NODEHEAD));
	if (!ptrNew){
		return NULL;
	}
	memcpy(ptrNew, ptrNode, sizeof(NODEHEAD));
	ptrNew->Next = NULL;
	return ptrNew;
}
template <typename NODEHEAD>
int GetNodeCount(const NODEHEAD *ListHead){
	int i = 1;
	const NODEHEAD *ptrHead = (NODEHEAD *)NULL;
	if (!ListHead)return 0;
	for (ptrHead = ListHead; ptrHead && i++; ptrHead = ptrHead->Next);
	return i - 1;
}
template <typename NODEHEAD>
NODEHEAD *GetNodeAddress(NODEHEAD *ListHead,const NODEHEAD *FindType, int Index, BOOL Last){
	if (!ListHead)return NULL;
	NODEHEAD *ptrHead = NULL, *ptrLast = NULL;
	if (Index > -1){
		int i = 0;
		if (Last){
			for (ptrHead = ListHead; ptrHead; ptrHead = ptrHead->Next){
				if (i++ == Index){
					if (ptrLast){
						ptrHead = ptrLast;
					}
					return ptrHead;
				}
				else{
					ptrLast = ptrHead;
				}
			}	
		}
		else{
			for (ptrHead = ListHead; ptrHead; ptrHead = ptrHead->Next){
				if (i++ == Index){
					return ptrHead;
				}
			}
		}
	}
	if (FindType){
		if (Last){
			for (ptrHead = ListHead; ptrHead; ptrHead = ptrHead->Next){
				if (!memcmp(ptrHead, FindType, sizeof(NODEHEAD))){
					if (ptrLast){
						ptrHead = ptrLast;
					}
					return ptrHead;
				}
				else{
					ptrLast = ptrHead;
				}
			}
		}
		else{
			for (ptrHead = ListHead; ptrHead; ptrHead = ptrHead->Next){
				if (!memcmp(ptrHead, FindType, sizeof(NODEHEAD) - 4)){
					return ptrHead;
				}
			}
		}
	}
	return (NODEHEAD *)NULL;
}
template <typename NODEHEAD>
NODEHEAD *InsertNode(NODEHEAD **ListHead, const NODEHEAD *ptrNode, int Index){
	if (Index < 0)return FALSE;
	NODEHEAD *ptrNew = CreateNode(ptrNode);
	if (!ptrNew)return FALSE;
	if (!(*ListHead) || Index < 1){
		ptrNew->Next = *ListHead;
		*ListHead = ptrNew;
		return ptrNew;
	}
	if (Index > GetNodeCount(*ListHead))Index = GetNodeCount(*ListHead);
	NODEHEAD *ptrHead = GetNodeAddress(*ListHead, (NODEHEAD *)nullptr, Index - 1, FALSE);
	if (!ptrHead)return FALSE;
	ptrNew->Next = ptrHead->Next;
	ptrHead->Next = ptrNew;
	return ptrNew;
}
template <typename NODEHEAD>
BOOL DeleteNode(NODEHEAD **ListHead, const NODEHEAD *ptrNode, int Index){
	if (!ListHead){
		return FALSE;
	}
	NODEHEAD *ptrLast = GetNodeAddress(*ListHead, ptrNode, Index, TRUE);
	NODEHEAD *ptrDeleNode = GetNodeAddress(*ListHead, ptrNode, Index, FALSE);
	if (!ptrDeleNode)return FALSE;
	if (ptrLast == ptrDeleNode){
		*ListHead = ptrDeleNode->Next;
		free(ptrDeleNode);
		ptrDeleNode = NULL;
		if (!GetNodeCount(*ListHead)){
			return FALSE;
		}
	}
	else{
		ptrLast->Next = ptrDeleNode->Next;
		free(ptrDeleNode);
		ptrDeleNode = NULL;
	}
	return TRUE;
}
template <typename NODEHEAD>
BOOL DestroyList(NODEHEAD **ListHead){
	if (!ListHead || !(*ListHead)){
		return FALSE;
	}
	while (DeleteNode(ListHead, *ListHead, -1));
	*ListHead = NULL;
	return TRUE;
}
#endif
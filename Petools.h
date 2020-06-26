// Petools.h: interface for the Petools class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PETOOLS_H__E70A34E2_D90B_4345_AB6E_9FE4CF7FC116__INCLUDED_)
#define AFX_PETOOLS_H__E70A34E2_D90B_4345_AB6E_9FE4CF7FC116__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

DWORD CopyFileBufferToImageBuffer(PVOID pFileBuffer,PVOID pImageBuffer);		
DWORD CopyImageBufferToNewBuffer(PVOID pImageBuffer,PVOID* pNewBuffer);
void FileBufferToAddShellcode(PVOID pFileBuffer);
void AddNewSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer);
void ExpandSection(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer);
void printfPE(PVOID pFileBuffer);
void PrintRelocation(PVOID pFileBuffer); //��ӡ�ض�λ��
DWORD FOA_TO_RVA(PVOID FileAddress, DWORD FOA,PDWORD pRVA);
DWORD RVA_TO_FOA(PVOID FileAddress, DWORD RVA, PDWORD pFOA);
void MyReadFile(PVOID* pFileBuffer,PDWORD BufferLenth, TCHAR* szFilePath);
void MyWriteFile(PVOID pMemBuffer,DWORD BufferLenth);
int GetBufferLength(PVOID Buffer);
void PrintfImportTable(PVOID pFileBuffer); //��ӡ�����
void MoveExportTable(PVOID pFileBuffer, PDWORD OldBufferSize,PVOID* pNewBuffer); //�ƶ�������
void MoveRelocationTable(PVOID pFileBuffer, PDWORD OldBufferSize,PVOID* pNewBuffer); //�ƶ��ض�λ��
void PrintBindImportTable(PVOID pFileBuffer); //��ӡ�󶨵����
void MoveAndInjectImportTable(PVOID pFileBuffer,PDWORD OldBufferSize,PVOID* pNewBuffer); //�ƶ���������ҳ��Խ���ע��
void printfResourceTable(PVOID pFileBuffer);

#endif // !defined(AFX_PETOOLS_H__E70A34E2_D90B_4345_AB6E_9FE4CF7FC116__INCLUDED_)

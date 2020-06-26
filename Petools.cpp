// Petools.cpp: implementation of the Petools class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Petools.h"
#include<STDLIB.H>
#include<STDIO.H>
#include<WINDOWS.H>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void MyReadFile(PVOID* pFileBuffer,PDWORD BufferLenth, TCHAR* szFilePath){
	FILE* File;
	File = fopen(szFilePath,"rb");
	
	if(File == NULL){
		printf("�ļ������ʧ��");
		return;
	}
	
	//��ȡ�ļ�
	fseek(File,0,SEEK_END);
	*BufferLenth = ftell(File);
	
	//���°�Fileָ��ָ���ļ��Ŀ�ͷ
	fseek(File,0,SEEK_SET);
	
	//�����¿ռ�
	*pFileBuffer = (PVOID)malloc(*BufferLenth);
	
	//�ڴ�����
	memset(*pFileBuffer,0,*BufferLenth);
	
	//��ȡ���ڴ滺����
	fread(*pFileBuffer,*BufferLenth,1,File);// һ�ζ���*bufferlenth���ֽڣ��ظ�1��
	
	//�ر��ļ����
	fclose(File);
}

//FOA_TO_RVA:FOA ת�� RVA							
DWORD FOA_TO_RVA(PVOID FileAddress, DWORD FOA,PDWORD pRVA)
{
	int ret = 0;
	int i;
	
	PIMAGE_DOS_HEADER pDosHeader				= (PIMAGE_DOS_HEADER)(FileAddress);
	PIMAGE_FILE_HEADER pFileHeader				= (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader	= (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionGroup			= (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	
	//RVA���ļ�ͷ�� �� SectionAlignment ���� FileAlignment ʱRVA����FOA
	if (FOA < pOptionalHeader->SizeOfHeaders || pOptionalHeader->SectionAlignment == pOptionalHeader->FileAlignment)
	{
		*pRVA = FOA;
		return ret;
	}
	
	//ѭ���ж�FOA�ڽ�����
	for (i=0;i < pFileHeader->NumberOfSections; i++)
	{
		if (FOA >= pSectionGroup[i].PointerToRawData && FOA < pSectionGroup[i].PointerToRawData + pSectionGroup[i].SizeOfRawData)
		{
			*pRVA = FOA - pSectionGroup[i].PointerToRawData + pSectionGroup[i].VirtualAddress;
			return *pRVA;
		}
	}
	
	//û���ҵ���ַ
	ret = -4;
	printf("func FOA_TO_RVA() Error: %d ��ַת��ʧ�ܣ�\n", ret);
	return ret;
}

//���ܣ�RVA ת�� FOA
// RVA_TO_FOA(pFileBuffer,pOptionHeader->DataDirectory[5].VirtualAddress,&FOA);
DWORD RVA_TO_FOA(PVOID FileAddress, DWORD RVA, PDWORD pFOA)
{
	int ret = 0;
	int i=0;
	PIMAGE_DOS_HEADER pDosHeader				= (PIMAGE_DOS_HEADER)(FileAddress);
	PIMAGE_FILE_HEADER pFileHeader				= (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader	= (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionGroup			= (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pFileHeader->SizeOfOptionalHeader);
	
	
	//RVA���ļ�ͷ�� �� SectionAlignment(�ڴ����) ���� FileAlignment(�ļ�����) ʱ RVA����FOA
	if (RVA < pOptionalHeader->SizeOfHeaders || pOptionalHeader->SectionAlignment == pOptionalHeader->FileAlignment)
	{
		// 37000
		*pFOA = RVA;
		return ret;
	}
	
	/*
		��һ����ָ����.VirtualAddress <= RVA <= ָ����.VirtualAddress + Misc.VirtualSize(��ǰ���ڴ�ʵ�ʴ�С)
		�ڶ�������ֵ = RVA - ָ����.VirtualAddress
		��������FOA = ָ����.PointerToRawData + ��ֵ
	*/

	//ѭ���ж�RVA�ڽ�����
	for (i=0;i<pFileHeader->NumberOfSections; i++)
	{
		// RVA > ��ǰ�����ڴ��е�ƫ�Ƶ�ַ ���� RVA < ��ǰ�ڵ��ڴ�ƫ�Ƶ�ַ+�ļ�ƫ�Ƶ�ַ
		if (RVA >= pSectionGroup[i].VirtualAddress && RVA < pSectionGroup[i].VirtualAddress + pSectionGroup[i].Misc.VirtualSize)
		{
			*pFOA =  RVA - pSectionGroup[i].VirtualAddress + pSectionGroup[i].PointerToRawData;
			return ret;
		}
	}
	
	//û���ҵ���ַ
	ret = -4;
	printf("func RVA_TO_FOA() Error: %d ��ַת��ʧ�ܣ�\n", ret);
	return ret;
}

// ���ܣ���ӡPE�ṹ
void printfPE(PVOID pFileBuffer){
    PIMAGE_DOS_HEADER pDosHeader = NULL;    
    PIMAGE_NT_HEADERS pNTHeader = NULL; 
    PIMAGE_FILE_HEADER pPEHeader = NULL;    
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;  
    PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
	PVOID AddressOfNamesTable = NULL;
	DWORD AddressOfNameOrdinalsNumber = NULL;
	PVOID FunctionOfAddress = NULL;
	char FunName[10] = {0};
	int i,j;

	DWORD FOA;
	char SectionName[9] = {0};

    pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER); 
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + IMAGE_SIZEOF_NT_OPTIONAL_HEADER);


    //�ж��Ƿ�����Ч��MZ��־��Ҳ����0x5A4D��ȡǰ�ĸ��ֽ�
    if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)    
    {   
        printf("������Ч��MZ��־\n");
        free(pFileBuffer);
        return ; 
    }   
	

    
    //��ӡDOSͷ    
    printf("********************DOSͷ********************\n\n"); 
    printf("_IMAGE_DOS_HEADERMZ->e_magic MZ��־��0x%x\n",pDosHeader->e_magic);
    printf("_IMAGE_DOS_HEADERMZ->e_lfanewָ��PE��־��0x%x\n",pDosHeader->e_lfanew);
    printf("\n");
	
    //�ж��Ƿ�����Ч��PE��־  
    if(*((PDWORD)((DWORD)pFileBuffer+pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)  
    {   
        printf("������Ч��PE��־\n");
        free(pFileBuffer);
        return ;
    }   
	
    
    //��ӡNTͷ 
    printf("********************NTͷ********************\n\n");  
    printf("_IMAGE_NT_HEADERS->Signature�ļ�PE��ʶ��0x%x\n",pNTHeader->Signature);
    printf("\n");
	

    printf("********************PEͷ********************\n\n");  
    printf("_IMAGE_FILE_HEADER->Machine֧�ֵ�CPU��0x%x\n",pPEHeader->Machine);
    printf("_IMAGE_FILE_HEADER->NumberOfSections�ڵ�������0x%x\n",pPEHeader->NumberOfSections);
    printf("_IMAGE_FILE_HEADER->SizeOfOptionalHeader��ѡPEͷ�Ĵ�С��0x%x\n",pPEHeader->SizeOfOptionalHeader);
    printf("\n");

	
    printf("********************OPTIOIN_PEͷ********************\n\n");  
    printf("_IMAGE_OPTIONAL_HEADER->Magic�ֱ�ϵͳλ��:0x%x\n",pOptionHeader->Magic);
    printf("_IMAGE_OPTIONAL_HEADER->AddressOfEntryPoint�������:0x%x\n",pOptionHeader->AddressOfEntryPoint);
    printf("_IMAGE_OPTIONAL_HEADER->ImageBase�ڴ澵���ַ:0x%x\n",pOptionHeader->ImageBase);
    printf("_IMAGE_OPTIONAL_HEADER->SectionAlignment�ڴ�����С:0x%x\n",pOptionHeader->SectionAlignment);
    printf("_IMAGE_OPTIONAL_HEADER->FileAlignment�ļ������С:0x%x\n",pOptionHeader->FileAlignment);
    printf("_IMAGE_OPTIONAL_HEADER->SizeOfImage�ڴ���PE�Ĵ�С(SectionAlignment������):0x%x\n",pOptionHeader->SizeOfImage);
    printf("_IMAGE_OPTIONAL_HEADER->SizeOfHeadersͷ+�ڱ����ļ�����Ĵ�С:0x%x\n",pOptionHeader->SizeOfImage);
    printf("_IMAGE_OPTIONAL_HEADER->NumberOfRvaAndSizesĿ¼����Ŀ:0x%x\n",pOptionHeader->NumberOfRvaAndSizes);
	
    printf("\n");
	
    //�ڱ�
    printf("********************�ڱ�********************\n\n");
    
    for(i=1;i<=pPEHeader->NumberOfSections;i++){
        char SectionName[9] ={0};
        strcpy(SectionName,(char *)pSectionHeader->Name);
        printf("_IMAGE_SECTION_HEADER->Name:%s\n",SectionName);
        printf("_IMAGE_SECTION_HEADER->VirtualSize:0x%x\n",pSectionHeader->Misc);
        printf("_IMAGE_SECTION_HEADER->VirtualAddress:0x%x\n",pSectionHeader->VirtualAddress);
        printf("_IMAGE_SECTION_HEADER->SizeOfRawData:0x%x\n",pSectionHeader->SizeOfRawData);
        printf("_IMAGE_SECTION_HEADER->PointerToRawData:0x%x\n",pSectionHeader->PointerToRawData);
        printf("_IMAGE_SECTION_HEADER->Characteristics:0x%x\n",pSectionHeader->Characteristics);
        pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pSectionHeader + IMAGE_SIZEOF_SECTION_HEADER);
        printf("\n");
    }


	RVA_TO_FOA(pFileBuffer,pOptionHeader->DataDirectory[0].VirtualAddress,&FOA);
	
	//������ĵ�ַ
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + FOA);
	
	//Ŀ¼���е�16�ű�Ľ���
	
	//�Ƚ�����һ�ű�������
	printf("********************������********************\n\n");
	printf("������������ַ:%x\n",pOptionHeader->DataDirectory[0].VirtualAddress);
	printf("������Ĵ�С:%x\n",pOptionHeader->DataDirectory[0].Size);
	printf("_IMAGE_EXPORT_DIRECTORY->Characteristics: 0x%x\n",pExportDirectory->Characteristics);
	printf("_IMAGE_EXPORT_DIRECTORY->TimeDateStampʱ���: 0x%x\n",pExportDirectory->TimeDateStamp);
	printf("_IMAGE_EXPORT_DIRECTORY->MajorVersion: 0x%x\n",pExportDirectory->MajorVersion);
	printf("_IMAGE_EXPORT_DIRECTORY->MinorVersion: 0x%x\n",pExportDirectory->MinorVersion);
	printf("_IMAGE_EXPORT_DIRECTORY->Nameָ��õ������ļ����ַ���: 0x%x\n",pExportDirectory->Name);
	printf("_IMAGE_EXPORT_DIRECTORY->Base����������ʼ���: 0x%x\n",pExportDirectory->Base);
	printf("_IMAGE_EXPORT_DIRECTORY->NumberOfFunctions���е��������ĸ���: 0x%x\n",pExportDirectory->NumberOfFunctions);
	printf("_IMAGE_EXPORT_DIRECTORY->NumberOfNames�Ժ������ֵ����ĺ�������: 0x%x\n",pExportDirectory->NumberOfNames);
	printf("_IMAGE_EXPORT_DIRECTORY->RVA_AddressOfFunctions����������ַ��: 0x%x\n",pExportDirectory->AddressOfFunctions);
	printf("_IMAGE_EXPORT_DIRECTORY->RAV_AddressOfNames�����������Ʊ�: 0x%x\n",pExportDirectory->AddressOfNames);
	printf("_IMAGE_EXPORT_DIRECTORY->RVA_AddressOfNameOrdinals����������ű�: 0x%x\n",pExportDirectory->AddressOfNameOrdinals);	

	printf("\n");




	//1�������������Ʊ���Ѱ�ҵ���������ַ��AddressOfNames��һ��ָ�������Ƶ�RVA��ַ����Ҫ��ת��Ϊ �ļ�ƫ�Ƶ�ַ
	RVA_TO_FOA(pFileBuffer,pExportDirectory->AddressOfNames,&FOA);

	//printf("pExportDirectory->AddressOfNames�����������Ʊ�: 0x%x\n",FOA);

	//2���ټ���pFileBuffer��ת��Ϊ�ļ���ַ���õ��������ƴ洢�ĵط����׵�ַ����ǰ���׵�ַ��RVA��Ҳ��Ҫ����RVA -> FOAת��
	AddressOfNamesTable = (PVOID)(*(PDWORD)((DWORD)pFileBuffer+(DWORD)FOA)); 
	RVA_TO_FOA(pFileBuffer,(DWORD)AddressOfNamesTable,&FOA); // // �����������Ʊ��к������Ƶ�FOA

	//AddressOfNamesTable = (PVOID)FOA;
	AddressOfNamesTable = (PVOID)((DWORD)pFileBuffer + (DWORD)FOA); // ����pFileBufferλ�þ͵��������ĺ������Ʊ�ĵ�ַ
	printf("\n");
	
	//3���õ��������Ʊ���ļ���ַ��ÿ������������ ռ�ĸ��ֽڣ�Ȼ����б����ж�	
	for(j=0;j<pExportDirectory->NumberOfNames;j++){
		//(PDWORD)((DWORD)AddressOfNamesTable + 4*j);
		//��ȡ��ǰ�������Ʊ��еĺ������ƣ�Ȼ��ѭ���ж�
		//printf("this is my test:%s \n", (PVOID)((DWORD)AddressOfNamesTable));
		strcpy(FunName,(PCHAR)((DWORD)AddressOfNamesTable)); //����+1 �����һ���ֽ�Ϊ���ֽ� ��ô��Ϊ������
		if(0 == memcmp((PDWORD)((DWORD)AddressOfNamesTable),(PDWORD)FunName,strlen(FunName))){
			AddressOfNamesTable = (PVOID)((DWORD)AddressOfNamesTable + (DWORD)(strlen((PCHAR)AddressOfNamesTable)+1));			
			//4���ҵ���ű�AddressOfNameOrdinals�±�����Ӧ�ĵ�ֵ����ű���ÿ����Առ2�ֽ� word����
			RVA_TO_FOA(pFileBuffer,pExportDirectory->AddressOfNameOrdinals,&FOA);
			AddressOfNameOrdinalsNumber = *(PWORD)((DWORD)FOA + (DWORD)pFileBuffer + (DWORD)j*2);
			//5��ͨ����ű����±���õ�ֵȥ����������ַ��AddressOfFunctions��Ѱ�� ��ֵ�±��Ӧ��ֵ
			RVA_TO_FOA(pFileBuffer,pExportDirectory->AddressOfFunctions,&FOA);
			printf("�������: %d\t",AddressOfNameOrdinalsNumber);
			printf("��������Ϊ: %s\t",FunName);
			printf("����������ַ��ĵ�ַΪ��0x%.8x\n",*(PDWORD)(PVOID)((DWORD)FOA + (DWORD)pFileBuffer + AddressOfNameOrdinalsNumber*4));
		}
	}
	
	printf("\n");

	printf("********************�����********************\n\n");
	printf("�����������ַ:%x\n",pOptionHeader->DataDirectory[1].VirtualAddress);
	printf("�����Ĵ�С:%x\n",pOptionHeader->DataDirectory[1].Size);

	
	printf("\n");

	printf("********************��Դ��********************\n\n");
	printf("��Դ��������ַ:%x\n",pOptionHeader->DataDirectory[2].VirtualAddress);
	printf("��Դ��Ĵ�С:%x\n",pOptionHeader->DataDirectory[2].Size);
	printf("\n");

    //�ͷ��ڴ�  
    free(pFileBuffer);  
}

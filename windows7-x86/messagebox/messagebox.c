#include<stdio.h>
#include<stdlib.h>


int main()
{
	
	void *ker_add = NULL;

	__asm
	{
		mov eax, fs:[0x30]  //PEB
		mov eax, [eax + 0xc]  //PEB->LDR   /*00C*/ VOID *DllList;
		mov esi, [eax + 0x14] //PEB->LDR.InMemOrderModuleList
		lodsd               //Eax=second module of InMemOrderModuleList (ntdll.dll)
		xchg esi, eax
		lodsd               //Eax=third module of InMemOrderModuleList (kernel32.dll)
		mov ecx, [eax + 0x10] //kernel32 base address
		mov ker_add, ecx
		// ecx = kernel32 base

		xor ebx, ebx
		mov ebx, [ecx + 0x3c]    //(kernel32.dll base address+0x3c)=DOS->e_lfanew
		add ebx, ecx             //(DOS->e_lfanew+base address of kernel32.dll)=PE Header
		mov ebx, [ebx + 0x78]    // DataDirectory->VirtualAddress
		add ebx, ecx             // IMAGE_EXPORT_DIRECTORY
		mov esi, [ebx + 0x20]    //AddressOfNames 函数名的列表 offset
		add esi, ecx             //AddressOfNames 函数名的列表 address		

		xor edx,edx
		Get_func:
			inc edx				//; Incrementing the Ordinal
			lodsd				//; Get name Offset
			add eax, ecx		//; (name offset + kernel32.dll base address) = Get Function name
			cmp dword ptr[eax], 0x50746547	//; PteG
			jnz Get_func
			cmp dword ptr[eax + 0x4], 0x41636f72 //; Acor
			jnz Get_func
			cmp dword ptr[eax + 0x8], 0x65726464 //; erdd
			jnz Get_func		
		mov GetProcAddre_ptr,edx
		
		mov esi,[ebx + 0x1c]    //;(IMAGE_EXPORT_DIRECTORY+0x1c)=AddressOfFunctions
		add esi,ecx				//;ESI=beginning of Address table	
		mov edx,[esi + edx*4]	//;EDX=Pointer(offset)
		add edx,ecx				//;GetProcAddress()
		
		mov esi,edx
		mov edi,ecx
		
		//finding address of LoadLibraryA()
		xor ebx,ebx
		push ebx				// '\x00'
		push 0x41797261			//Ayra
		push 0x7262694c			//rbiL
		push 0x64616f4c			//daoL
		push esp		//esp = LoadLibraryA()
		push ecx		//ecx = kernel32.dll
		call edx		//GetProcAddress(HMODULE hModule,LPCSTR lpProcName) (DLL,func_name)
						//eax = LoadLibraryA()
		add esp, 16
		

		//Load library user32.dll
		xor ecx, ecx			
		push 0x00006c6c		//ll
		//mov byte ptr ss:[esp + 2], cl	//cl = ecx的低16bit     ,此行目的,只是為了要加入截斷字元
		push 0x642e3233		//d.23
		push 0x72657375		//resu
		push esp
		call eax			//LoadLibraryA("user32.dll")
		add esp, 12

		//Finding address of MessageBoxA()
		xor ecx, ecx
		push 0x0041786f					//Axo
		//mov byte ptr ss:[esp + 3],cl
		push 0x42656761					//Bega
		push 0x7373654d					//sseM
		push esp
		push eax						//eax=MessageBoxA()
		call esi						//esi=GetProcAddress("user32.dll","MessageBoxA()")		
		add esp, 12

			
		//;MessageBoxA(NULL,"test","test",1)
		xor edx, edx
		xor ecx, ecx
		push edx
		push 0x74736574
		lea edx, [esp]		
		push ecx
		push 0x74736574
		lea ecx, [esp]		
		xor ebx, ebx
		inc ebx				//1
		push ebx
		push edx
		push ecx
		xor ebx, ebx
		push ebx
		call eax


		//ExitProcess()
		xor ecx, ecx
		push 0x00737365					//sse   
		//mov byte ptr ss:[esp + 3],cl
		push 0x636f7250					//corP
		push 0x74697845					//tixE
		lea ecx, [esp]


		push ecx
		push edi	

		call esi	//GetProcAddress("user32.dll",ExitProcess)

		
		xor ecx, ecx
		push ecx
		call eax	//ExitProcess(0)
	}
	
	printf("kernel32 address : %p\n", ker_add);

	 
	system("pause");
	return 0;
}

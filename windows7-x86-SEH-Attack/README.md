# windows7(x86) SEH Attack

## Reference

> 這篇筆記,是基於參考下列所完成的筆記,詳細請參考以下網址
+ 1.http://securityalley.blogspot.tw/2014/11/blog-post.html
+ 2.https://www.exploit-db.com/exploits/40245/
+ 3.wiki

## 前言

SEH(structured exception handling)中文叫**例外處理機制**,就是當process遇到例外的時候,會去呼叫的處理機制,

例如C裡面可以用try來處理例外,但實際上例外處理還是由OS來做。


### 例外處理的攻擊原理

SEH對每個user Thread,都有一個list來處理異常事件,該list的每個異常處理幀由兩個成員組成,分別是list的 **下一個例外處理機制的地址**、**當前處理函式的地址** 這邊用next、handler代表兩者,當例外發生的時候,OS會從 **FS:[0]** 也就是 TEB 找到例外處理函式的list,然後從頭開始,一個一個去呼叫例外處理函式,直到有人處裡為止。

![SEH](https://github.com/qqgnoe466263/shellcode/blob/master/windows7-x86-SEH-Attack/pic/SEH.png)



### 實作

> 可以參考以下步驟找到kernel32.dll在memory的位址
+ (1)先用FS暫存器找到**TEB(Thread Environment Block)**
+ (2)在TEB中偏移0x30的地方放著**PEB(Process Envirorment Block)**的指標
+ (3)在PEB中偏移0xC的地方放著**PEB_LDR_DATA結構**的指標
+ (4)在PEB_LDR_DATA中偏移0x14放著**InInitializationOrderModuleList**
+ (5)而在InInitializationOrderModuleList這個list中放的就是PE載入的DLL,第一個node是ntdll.dll,第二個node是kernel32.dll
+ (6)找到kernel32.dll的node後再偏移0x10就是kernel32.dll在memory中的base address

		mov eax, fs:[0x30]    //PEB
		mov eax, [eax + 0xc]  //PEB->LDR 
		mov esi, [eax + 0x14] //PEB->LDR.InMemOrderModuleList
		lodsd                 //Eax=module of InMemOrderModuleList (ntdll.dll)
		xchg esi, eax
		lodsd                 //Eax= module of InMemOrderModuleList (kernel32.dll)
		mov ecx, [eax + 0x10] //kernel32 base address
 
 所以從我的電腦上找到的位置就是像這樣,可以用ollydbg或其他的動態調試工具看一下
 
![kernel32base](https://github.com/qqgnoe466263/shellcode/blob/master/windows7-x86/pic/ker_base.jpg)

> 有了kernel32.dll的位址就可以找GetProcAddress()的位址
+ (1)在kernel32.dll偏移0x3C的位址就是**PE Header**
+ (2)PE header偏移0x78的位址放著**IMAGE_EXPORT_DIRECTORY(函數導出目錄)**
+ (3)就可以從這個目錄找到所需函數的入口地址
+ (4)在目錄偏移0x20的地方放著**函數名稱表**,所以先從這個表找到GetProcAddress()的編號
+ (5)在目錄偏移0x1c的地方放著**函數偏移表(RVA)**,剛剛找到的編號拿來查詢這個表,就可以找到函數的偏移地址

		xor ebx, ebx
		mov ebx, [ecx + 0x3c]    //(kernel32.dll base address+0x3c)=DOS->e_lfanew
		add ebx, ecx             //(DOS->e_lfanew+base address of kernel32.dll)=PE Header
		mov ebx, [ebx + 0x78]    // DataDirectory->VirtualAddress
		add ebx, ecx             // IMAGE_EXPORT_DIRECTORY
		mov esi, [ebx + 0x20]    //AddressOfNames 函数名的列表 offset
		add esi, ecx             //AddressOfNames 函数名的列表 address		

		xor edx,edx
		Get_func:
			inc edx					//; Incrementing the Ordinal
			lodsd					//; Get name Offset
			add eax, ecx				//; (name offset + kernel32.dll base address) = Get Function name
			cmp dword ptr[eax], 0x50746547		//; PteG
			jnz Get_func
			cmp dword ptr[eax + 0x4], 0x41636f72 	//; Acor
			jnz Get_func
			cmp dword ptr[eax + 0x8], 0x65726464 	//; erdd
			jnz Get_func		
		mov GetProcAddre_ptr,edx
		
		mov esi,[ebx + 0x1c]    		//;(IMAGE_EXPORT_DIRECTORY+0x1c)=AddressOfFunctions
		add esi,ecx				//;ESI=beginning of Address table	
		mov edx,[esi + edx*4]			//;EDX=Pointer(offset)
		add edx,ecx				//;GetProcAddress()

有了GetProcAddress()這個function,就可以利用它執行其他的function!!

### Messegebox

OS: windows7 x64 

IDE: VS2015 Debug x86

![messagebox](https://github.com/qqgnoe466263/shellcode/blob/master/windows7-x86/pic/messagebox.jpg)

# windows7(x86) shellcode programming

## Reference

> 這篇筆記,是基於參考下列所完成的筆記,詳細請參考以下網址
+ 1.https://www.exploit-db.com/exploits/39900/
+ 2.https://www.exploit-db.com/google-hacking-database/
+ 3.0day2

## 前言

由於不同的OS,會影響Dynamic Library載入的位置,所以用靜態函數寫的shellcode會受到限制,

所以需要動態的去找到DLL載入的位置還有function的偏移大小。

### 動態定位API

Windows的API都是由DLL中的函數來實現的,而所有win32的程式都會先載入 **ntdll.dll** 跟 **kernel32.dll** 這兩個DLL。

所以思路就是先找到 **kernel32.dll的地址** 跟 **GetProcAddress()** 這個function的地址,再利用GetProcAddress()就可以使用其他函數了。

### 實作

> 可以參考以下步驟找到kernel32.dll中的API位址
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
 
 所以從我的電腦上找到的位置就是像這樣
 
![kernel32base]()




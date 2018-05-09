# windows7(x86) shellcode programming

## 前言

由於不同的OS,會影響Dynamic Library載入的位置,所以用靜態函數寫的shellcode會受到限制,

所以需要動態的去找到DLL載入的位置還有function的偏移大小。

### 動態定位API

Windows的API都是由DLL中的函數來實現的,而所有win32的程式都會先載入 **ntdll.dll** 跟 **kernel32.dll** 這兩個DLL

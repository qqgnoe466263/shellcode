# windows7(x86) shellcode programming

## 前言

由於不同的OS,會影響Dynamic Library載入的位置,所以用靜態函數寫的shellcode會受到限制,

所以需要動態的去找到DLL載入的位置還有function的偏移大小。


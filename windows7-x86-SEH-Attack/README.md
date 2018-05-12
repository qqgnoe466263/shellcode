# windows7(x86) SEH Attack

## Reference

> 這篇筆記,是基於參考下列所完成的筆記,詳細請參考以下網址
+ 1.<a href="http://securityalley.blogspot.tw/2014/11/blog-post.html">緩衝區溢位攻擊：第五章 - 攻擊的變化</a>
+ 2.<a href="https://www.exploit-db.com/exploits/40245/">Exploit-db</a>
+ 3.wiki

## 前言

SEH(structured exception handling)中文叫**例外處理機制**,就是當process遇到例外的時候,會去呼叫的處理機制,

例如C裡面可以用try來處理例外,但實際上例外處理還是由OS來做。


### 例外處理的攻擊原理

SEH對每個user Thread,都有一個list來處理異常事件,該list的每個異常處理幀由兩個成員組成,分別是list的 **下一個例外處理機制的地址**、**當前異常處理函式的地址** 這邊用next、handler代表兩者,當例外發生的時候,OS會從 **FS:[0]** 也就是 TEB 找到例外處理函式的list,然後從頭開始,一個一個去呼叫例外處理函式,直到有人處裡為止。

參考至<a href="http://securityalley.blogspot.tw/2014/11/blog-post.html">緩衝區溢位攻擊：第五章 - 攻擊的變化</a>文中此圖
![SEH](https://github.com/qqgnoe466263/shellcode/blob/master/windows7-x86-SEH-Attack/pic/SEH.png)

而思路就是,因為SEH是放在stack上的,所以可以利用overflow把SEH原本應該要去的**異常處裡函式**改成shellcode,
原本overflow就是一種異常事件,所以windows在處理異常的時候,就會把shellcode當成異常處裡函式執行了。

### 實作

使用範例也是參考<a href="http://securityalley.blogspot.tw/2014/11/blog-post.html">緩衝區溢位攻擊：第五章 - 攻擊的變化</a>只是我自己稍作了一些跟改,方便我自己實作,也配合自己的環境。

    #include<stdio.h>
    #include<stdlib.h>
    #include<windows.h>

    void seh()
    {
      __asm {
        pop ebx
        pop ebp
        ret
      }
    }

    void do_something(FILE *pfile)
    {
      char buf[128];
      fscanf(pfile, "%s", buf);
    }

    int main(int argc, char **argv)
    {
      char dummy[512];
      FILE *pfile;
      printf("Vulnerable001 starts...\n");

      if (!(pfile = fopen("Vulnerable001_Excp_Exploit.txt", "r"))) {
        exit(0);
      }
      if (pfile) do_something(pfile);
      printf("Vulnerable001 ends....\n");
      system("pause");
    }
  
To me continue....


### Messegebox

OS: windows7 x64 

IDE: VS2015 Debug x86 關閉DEP,ALSR

一開始所使用的shellcode是<a href="https://www.exploit-db.com/exploits/40245/">Exploit-db</a>的,但發現會被00截斷後來還是放棄了-.-

![messagebox](https://github.com/qqgnoe466263/shellcode/blob/master/windows7-x86-SEH-Attack/pic/Messagebox.png)

# KMPlayer1440-OverFlow

## Reference

> 這篇筆記,是基於參考下列所完成的筆記,詳細請參考以下網址
+ 1.<a href="http://securityalley.blogspot.tw/2014/09/blog-post.html">緩衝區溢位攻擊：第四章 - 真槍實彈</a>

## 前言

這是一個在 **KMPlayer ver 3.0.0.1440** 上的緩衝區溢位攻擊,透過overflow達成任意代碼攻擊,攻擊者可以精心構造一個MP3檔給人下載(畢竟很多人愛聽盜版音樂),而MP3檔案是防毒軟體也不會檢查出來的,所以可以讓攻擊者取得控制權。

### 實作

首先,先測試一下是否有Overflow直接跟文中一樣灌10000個A給他

![kmp10000A](https://github.com/qqgnoe466263/shellcode/blob/master/KMPlayer1440-Attack/pic/kmp10000A.png)

然後用mona找一下會蓋到return address的offset

    !mona pattern_offset 61413561

我的offset只有16

![offset](https://github.com/qqgnoe466263/shellcode/blob/master/KMPlayer1440-Attack/pic/offset.png)

剩下的就是把stack疊好,就可以成功攻擊了!!在疊的時候因為stack的位置跟文中說的一樣EIP跟ESP隔了4bytes所以要先任意塞4bytes進去,在塞shellcode。

    |---------offset---------|---------retAdd---------|---------any 4bytes---------|---------ESP---------|

塞好大概是長這個樣子

    |---------offset---------|---------push esp;ret---------|---------any 4bytes---------|---------shellcode---------|


這樣就可以用 **push esp;ret;** 跳進去esp執行shellcode的。

### Messagebox

OS: windows7 x64 (關閉DEP)

KMPlayer 3.1440 ver

![messagebox](https://github.com/qqgnoe466263/shellcode/blob/master/KMPlayer1440-Attack/pic/message.png)

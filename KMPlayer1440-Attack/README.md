# KMPlayer1440-OverFlow

## Reference

> 這篇筆記,是基於參考下列所完成的筆記,詳細請參考以下網址
+ 1.<a href="http://securityalley.blogspot.tw/2014/09/blog-post.html">緩衝區溢位攻擊：第四章 - 真槍實彈</a>

## 前言

這是一個在 **KMPlayer ver 3.0.0.1440** 上的緩衝區溢位攻擊,透過overflow達成任意代碼攻擊,攻擊者可以精心構造一個MP3檔給人下載(畢竟很多人愛聽盜版音樂),而MP3檔案是防毒軟體也不會檢查出來的,所以可以讓攻擊者取得控制權。

### 實作

首先,先測試一下是否有Overflow直接跟文一樣灌10000個A給他

![kmp10000A](https://github.com/qqgnoe466263/shellcode/blob/master/KMPlayer1440-Attack/pic/kmp10000A.png)


### Messegebox

OS: windows7 x64 

IDE: VS2015 Debug x86 關閉DEP,ALSR


![messagebox](https://github.com/qqgnoe466263/shellcode/blob/master/KMPlayer1440-Attack/pic/message.png)

Translator
===

由于网络原因，API无法直接访问，请使用可设置 HTTP 代理的 [Node.js 版本](https://github.com/develon2015/Translator/tree/node)

安装
---
直接下载
```
wget https://github.com/develon2015/Translator/raw/master/bin_x64/tl
chmod +x tl
./tl hello
```
编译安装，请先安装gcc、make
```
git clone https://github.com/develon2015/Translator
make clean
make
make install
```

运行效果
---
```
$ tl -h
翻译程序
        tl [sl] [tl] <tc>
        交互式Shell中可以切换语种, 使用":sl tl"命令即可
        例如":en_US zh_CN"

        Sep 22 2019 - 09:33:24

$ tl わたし、プログラミングが好きなの
我喜欢编程

$ tl ja_JP zh_TW わたし、プログラミングが好きなの
我喜歡編程

$ tl en_US わたし、プログラミングが好きなの
I like programming

$ tl
Are you OK?
你还好吗？

:ja
[INFO]语种切换: auto -> ja.

Are you OK?
大丈夫ですか？

:auto zh_HK
[INFO]语种切换: auto -> zh_HK.

大丈夫ですか？
你還好嗎？

^D
```

项目结构
---

```Bash
.
├── bin_x64
│   └── tl
├── makefile
├── README.md
├── src
│   ├── main.c
│   └── uclib
│       ├── inc
│       │   └── net.h
│       ├── Makefile
│       ├── net
│       │   └── resolveIP.c
│       ├── README.md
│       └── uclib.a
└── translate.exe
```

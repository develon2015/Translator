Translator
===

运行效果
---
```
$ tl わたし、プログラミングが好きなの
翻译结果:"我喜欢编程"

$ tl ja_JP zh_TW わたし、プログラミングが好きなの
翻译结果:"我喜歡編程"

$ tl en_US わたし、プログラミングが好きなの
翻译结果:"I like programming"
```

文件一览
---

```Bash
.:
makefile  README.md  src  translate.exe

./src:
main.c  uclib

./src/uclib:
inc  Makefile  net  README.md  test  tools

./src/uclib/inc:
net.h  tools.h

./src/uclib/net:
resolveIP.c

./src/uclib/test:
resolveIP.c  showMem.c  test.c

./src/uclib/tools:
showMem.c
```

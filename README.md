Translator
===

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

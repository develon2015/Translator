translate: src/main.c src/uclib/uclib.a
	gcc -W -Wall $^  -o translate -Isrc

.PHONY: clean
clean:
	rm -rf translate

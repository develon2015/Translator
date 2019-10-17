DIR := bin_x64
BIN := $(DIR)/tl

$(BIN): src/main.c src/uclib/uclib.a | $(DIR)
	gcc -W -Wall $^  -o $(BIN) -Isrc

src/uclib/uclib.a:
	make -C src/uclib

$(DIR):
	mkdir $@

.PHONY: deploy
deploy:
	sudo cp $(BIN) /usr/bin

.PHONY: clean
clean:
	make -C src/uclib clean
	rm -rf $(DIR)

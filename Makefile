PROG = gd-tools
PREFIX ?= /usr

SHELL_FILES := $(wildcard **/gd-*.sh)
CPP_FILES := $(wildcard **/gd-*.cpp)
FONT_FILES := $(wildcard **/*.ttf)
EXTRA_FILES := $(wildcard **/*.dic)

BUILD_FILES := $(patsubst %, build/%, $(notdir $(basename $(CPP_FILES))))

EXEC_INSTALL_DEST := $(patsubst %, $(PREFIX)/bin/%, $(notdir $(basename $(SHELL_FILES) $(CPP_FILES))))
FONT_INSTALL_DEST := $(patsubst %, $(PREFIX)/share/fonts/$(PROG)/%, $(notdir $(FONT_FILES)))
DICT_INSTALL_DEST := $(patsubst %, $(PREFIX)/share/$(PROG)/%, $(notdir $(EXTRA_FILES)))

.PHONY: all
all: build $(BUILD_FILES)

build:
	@echo -e '\033[1;32mBuilding the program...\033[0m'
	mkdir -p build

build/gd-%: **/gd-%.cpp
	g++ -O2 -std=c++23 -Wall -Wextra -Wpedantic -Werror $< -lmarisa -o $@

$(PREFIX)/bin/gd-%: build/gd-%
	install -Dm755 $< $@

$(PREFIX)/bin/gd-%: **/gd-%.sh
	install -Dm755 $< $@

$(PREFIX)/share/fonts/$(PROG)/%.ttf: **/%.ttf
	install -Dm644 $< $@

$(PREFIX)/share/$(PROG)/%.dic: **/%.dic
	install -Dm644 $< $@

.PHONY: install
install: build $(EXEC_INSTALL_DEST) $(FONT_INSTALL_DEST) $(DICT_INSTALL_DEST)
	@echo -e '\033[1;32mInstalling the program...\033[0m'

.PHONY: uninstall
uninstall:
	@echo -e '\033[1;32mUninstalling the program...\033[0m'
	rm -- $(EXEC_INSTALL_DEST)
	rm -rf -- $(PREFIX)/share/$(PROG)
	rm -rf -- $(PREFIX)/share/fonts/$(PROG)

.PHONY: clean
clean:
	@echo -e '\033[1;32mCleaning up...\033[0m'
	rm -rf -- out build

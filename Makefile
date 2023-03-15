PROG = gd-tools
PREFIX ?= /usr

EXEC_FILES := $(wildcard **/gd-*)
FONT_FILES := $(wildcard **/*.ttf)
EXTRA_FILES := $(wildcard **/*.dic)

EXEC_DEST := $(patsubst %, $(PREFIX)/bin/%, $(notdir $(EXEC_FILES)))
FONT_DEST := $(patsubst %, $(PREFIX)/share/fonts/$(PROG)/%, $(notdir $(FONT_FILES)))
EXTRA_DEST := $(patsubst %, $(PREFIX)/share/$(PROG)/%, $(notdir $(EXTRA_FILES)))

.PHONY: all
all:
	@echo -e "\033[1;32mThis program doesn't need to be built. Run \"make install\".\033[0m"

$(PREFIX)/bin/gd-%: **/gd-%
	install -Dm755 $< $@

$(PREFIX)/share/fonts/$(PROG)/%.ttf: **/%.ttf
	install -Dm644 $< $@

$(PREFIX)/share/$(PROG)/%.dic: **/%.dic
	install -Dm644 $< $@

.PHONY: install
install: $(EXEC_DEST) $(FONT_DEST) $(EXTRA_DEST)
	@echo -e '\033[1;32mInstalling the program...\033[0m'

.PHONY: uninstall
uninstall:
	@echo -e '\033[1;32mUninstalling the program...\033[0m'
	rm -- $(EXEC_DEST)
	rm -rf -- $(PREFIX)/share/$(PROG)
	rm -rf -- $(PREFIX)/share/fonts/$(PROG)

.PHONY: clean
clean:
	@echo -e '\033[1;32mCleaning up...\033[0m'
	rm -rf -- out

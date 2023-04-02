


### Usage
`gd-marisa %GDWORD% %GDSEARCH% [PATH_TO_DIC_FILE]`. The path to the .dic file defaults to `/usr/share/gd-tools/words.dic`

### Dependencies
[marisa-trie](https://github.com/s-yata/marisa-trie). Unfortunately there is no AUR-package yet, but you should be able to use the binary without it.

### Compilation
`g++ -o createlinks createlinks.cpp -lmarisa`

### Building an own index from a set of words
`marisa-build < keyset.txt > keyset.dic` where keyset is a newline seperated list of words. More information at https://www.s-yata.jp/marisa-trie/docs/readme.en.html


![demo](https://user-images.githubusercontent.com/50422430/229341045-96283fc9-8ecb-49bb-a011-abd2d3e4e43e.gif)


### Usage
`gd-marisa %GDWORD% %GDSEARCH% [PATH_TO_DIC_FILE]`. The path to the .dic file defaults to `/usr/share/gd-tools/words.dic`

### Dependencies
[marisa-trie](https://github.com/s-yata/marisa-trie). Unfortunately there is no AUR-package yet, but you should be able to use the binary without it.

### Compilation
`g++ -o createlinks createlinks.cpp -lmarisa`

### Building an own index from a set of words
`marisa-build < keyset.txt > keyset.dic` where keyset is a newline seperated list of words. More information at https://www.s-yata.jp/marisa-trie/docs/readme.en.html

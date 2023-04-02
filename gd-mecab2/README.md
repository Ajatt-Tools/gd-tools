


### Dependencies
[marisa-trie](https://github.com/s-yata/marisa-trie)

### Compilation
`g++ -o createlinks createlinks.cpp -lmarisa`

### Usage
`createlinks %GDWORD% %GDSEARCH% PATH_TO_DIC_FILE`.

### Building index
`marisa-build < keyset.txt > keyset.dic` where keyset is a newline seperated list of words. More information at https://www.s-yata.jp/marisa-trie/docs/readme.en.html

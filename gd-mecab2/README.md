


### Dependencies
[marisa-trie](https://github.com/s-yata/marisa-trie) if you are lucky, the AUR package `mingw-w64-marisa` works too

### Compilation
`g++ -o createlinks createlinks.cpp -lmarisa`

### Usage
`createlinks %GDWORD% %GDSEARCH%`. No highlighting yet, just a quick preview.\
The path to `words.dic` in the source code has to be configured though probably.

### Building index
`marisa-build < keyset.txt > keyset.dic` where keyset is a newline seperated list of words. More information at https://www.s-yata.jp/marisa-trie/docs/readme.en.html

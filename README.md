# GoldenDitct tools

A set of helpful programs to enhance goldendict for immersion learning.

## Table of Contents
- [Installation](#installation)
- [gd-marisa](#gd-marisa)
- [gd-mecab](#gd-mecab)
- [gd-pics](#gd-pics)
- [gd-strokeorder](#gd-strokeorder)
- [gd-massif](#gd-massif)
- [gd-ankisearch](#gd-ankisearch)

## Installation
Run `sudo make install`.\
Open GoldenDict, press "Edit" > "Dictionaries" > "Programs" and add this script as type html and Command Line `<name of script> %GDWORD% %GDSEARCH%`.\
Optionally add arguments, such as: `gd-mecab %GDWORD% --user-dict <path> --font-size 20px`.\
Now this program is treated as a dictionary and you can add it under Dictionaries or Groups.

## gd-marisa

This script outputs the sentence with clickable characters and searches for the longest available dictionary entry (from a predefined list) beginning at that character.
For deinflection it currently relies on the hunspell dictionary by epistularum, available [here](https://github.com/epistularum/hunspell-ja-deinflection).
It also provides links of available entries of smaller substrings.

![demo](https://user-images.githubusercontent.com/50422430/229341045-96283fc9-8ecb-49bb-a011-abd2d3e4e43e.gif)

**Usage**

`gd-marisa %GDWORD% %GDSEARCH% [PATH_TO_DIC_FILE]`. The path to the .dic file defaults to `/usr/share/gd-tools/words.dic`

**Dependencies**

[marisa-trie](https://github.com/s-yata/marisa-trie). The official Arch Linux package is called [marisa](https://archlinux.org/packages/community/x86_64/marisa/), but it's already a dependency of goldendict.

**Compilation**

If you don't want to use the provided binary, you can also compile the program yourself with: 
```g++ -o createlinks createlinks.cpp -lmarisa```

**Building an own index from a set of words**

If you would like to make changes to found words, you can also create an own index from a newline seperated list of words (here called `keyset.txt`):
```marisa-build < keyset.txt > keyset.dic```

More information at https://www.s-yata.jp/marisa-trie/docs/readme.en.html

## gd-mecab

This script passes a sentence through mecab in order to make every part of the sentence clickable

https://user-images.githubusercontent.com/50422430/226139459-0c8bcf0e-e68f-491e-8171-bae3f50a7ae1.mp4

**Dependencies**

This script requires [MeCab](https://taku910.github.io/mecab/) and the IPA dictionary installed. \
If you are on an Arch Linux system you can simply install the AUR package `mecab-ipa` to obtain both.

**Command format**

Add this script to GoldenDict under "Dictionaries" > "Programs" (format HTML), like this:

```
gd-mecab --word %GDWORD% --sentence %GDSEARCH%
```

**Optional arguments**

* `--font-size SIZE` the font size to be used, e.g. `30px`.
* `--user-dict FILE` full path to the user_dic.dic file. This is done automatically if you install via make.

## gd-pics

This script shows the top 5 pictures from Bing images for the given search string.

![image](https://user-images.githubusercontent.com/50422430/224940994-eb3e1be6-9cd8-4776-88cf-850c78648e81.png)

## gd-strokeorder

This script shows the search string in the `KanjiStrokeOrders` font.

![screenshot](https://user-images.githubusercontent.com/69171671/224840590-b740a1b6-8526-49ed-b4cd-efe03689a132.png)

Download font: https://www.nihilist.org.uk/

**Arguments**

* `--max-len` `5` maximum size of the input string.
* `--font-size` `10rem` font size. It has to be large in order to see the stroke numbers.

## gd-massif
This script shows example sentences from https://massif.la/

![image](https://user-images.githubusercontent.com/50422430/226018360-e46605f0-2fb4-481c-801e-73aca84fae70.png)

## gd-ankisearch

This script searches Anki cards in your collection that contain %GDWORD%.

**Arguments:**

* `--field-name` `NAME` optional field to limit search to.
* `--deck-name` `NAME` optional deck to limit search to.

**Example invocation:**

```
gd-ankisearch --field-name VocabKanji %GDWORD%
```

# GoldenDict tools

A set of helpful programs to enhance goldendict for immersion learning.

## Table of Contents
- [Installation](#installation)
- [gd-marisa](#gd-marisa)
- [gd-mecab](#gd-mecab)
- [gd-images](#gd-images)
- [gd-strokeorder](#gd-strokeorder)
- [gd-handwritten](#gd-handwritten)
- [gd-massif](#gd-massif)
- [gd-ankisearch](#gd-ankisearch)

## Installation

First, [install goldendict-ng](https://tatsumoto-ren.github.io/blog/setting-up-goldendict.html).

### Pacman-based distros

Install [gd-tools-git](https://aur.archlinux.org/packages/gd-tools-git) from the AUR.

### Other distros

If you want to package `gd-tools` for your distro and know how to do it,
please create a pull request.

<details>

<summary>Manual installation</summary>

This way is not recommended
because it's easy to make a mistake during the process
and end up with a broken install.

1) To build the `gd-tools` you need to install `xmake` and `gcc` (13.1 and newer).
1) Install `mecab` (`mecab-git` from the AUR).
2) Run `./quickinstall.sh`.

   **Note:** You can run `./quickinstall.sh --local`
   to install the program locally (to `~/.local/` ).

</details>

## Setup

Open GoldenDict, press "Edit" > "Dictionaries" > "Programs" and add the installed executables.
Set type to `html`.
Command Line: `<name of script> --word %GDWORD% --sentence %GDSEARCH%`.
Optionally add arguments, such as: `gd-mecab --word %GDWORD% --sentence %GDSEARCH% --user-dict <path> --font-size 20px`.
These programs are treated as dictionaries and you can add them under "Dictionaries" or "Groups".

## gd-marisa

This script outputs the sentence with clickable characters
and searches for the longest available dictionary entry
(from a predefined list) beginning at that character.
For deinflection it currently relies on [rdricpp](https://github.com/Ajatt-Tools/rdricpp).
It also provides links of available entries of smaller substrings.

![demo](https://user-images.githubusercontent.com/50422430/229341045-96283fc9-8ecb-49bb-a011-abd2d3e4e43e.gif)

**Usage**

```
gd-marisa --word %GDWORD% --sentence %GDSEARCH% --path-to-dic [PATH_TO_DIC_FILE]
```

The path to the `.dic` is an optional argument and defaults to `/usr/share/gd-tools/marisa_words.dic`

**Dependencies**

[marisa-trie](https://github.com/s-yata/marisa-trie).
The official Arch Linux package is called [marisa](https://archlinux.org/packages/community/x86_64/marisa/),
but it's already a dependency of goldendict.

**Building an own index from a set of words**

If you would like to make changes to found words,
you can also create an own index from a newline-separated list of words (here called `keyset.txt`):

```
marisa-build < keyset.txt > keyset.dic
```

More information at https://www.s-yata.jp/marisa-trie/docs/readme.en.html

## gd-mecab

This script passes a sentence through mecab in order to make every part of the sentence clickable.

https://user-images.githubusercontent.com/50422430/226139459-0c8bcf0e-e68f-491e-8171-bae3f50a7ae1.mp4

**Dependencies**

This script requires [MeCab](https://taku910.github.io/mecab/) and the IPA dictionary to be installed.
If you are on an Arch Linux system you can simply install the AUR package `mecab-ipadic` to obtain both.

**Command format**

Add this script to GoldenDict under "Dictionaries" > "Programs" (format HTML), like this:

```
gd-mecab --word %GDWORD% --sentence %GDSEARCH%
```

**Optional arguments**

* `--font-size SIZE` the font size to be used, e.g. `30px`.
* `--user-dict FILE` full path to the user_dic.dic file. This is done automatically if you install via make.

## gd-images

This script shows the top 5 pictures from Bing images for the given search string.

![image](https://user-images.githubusercontent.com/50422430/224940994-eb3e1be6-9cd8-4776-88cf-850c78648e81.png)

```
gd-images --word %GDWORD%
```

## gd-strokeorder

This script shows the search string in the `KanjiStrokeOrders` font.

```
gd-strokeorder --word %GDWORD%
```

![screenshot](https://user-images.githubusercontent.com/69171671/224840590-b740a1b6-8526-49ed-b4cd-efe03689a132.png)

Font source: https://www.nihilist.org.uk/

**Arguments**:

* `--max-len` `5` maximum size of the input string.
* `--font-size` `10rem` font size. It has to be large in order to see the stroke numbers.

**How to call**:

```
gd-strokeorder --word %GDWORD%
```

## gd-handwritten

This script displays the handwritten form of each character

```
gd-handwritten --word %GDWORD% --font-size 4rem
```

![screenshot](https://user-images.githubusercontent.com/53496273/254959907-4aa7d72d-2d58-4bd3-8b3a-c2f6f3186834.png)

Font source: [ArmedLemon](https://github.com/Ajatt-Tools/gd-tools/blob/main/res/ArmedLemon.ttf).

**How to call**:

```
gd-handwritten --word %GDWORD%
```

## gd-massif

This script shows example sentences from https://massif.la/

```
gd-massif --word %GDWORD%
```

![image](https://user-images.githubusercontent.com/50422430/226018360-e46605f0-2fb4-481c-801e-73aca84fae70.png)

## gd-ankisearch

This script searches Anki cards in your collection that contain %GDWORD%.

![screenshot](https://github.com/Ajatt-Tools/gd-tools/assets/69171671/45a6b3a1-97de-439c-8f17-bd6f81d81d73)

**Arguments:**

* `--field-name` `NAME` optional field to limit search to.
* `--deck-name` `NAME` optional deck to limit search to.
* `--show-fields` `VocabKanji,SentKanji` optional comma-separated list of fields to show.

**Example invocation:**

```
gd-ankisearch --field-name VocabKanji --show-fields VocabKanji,SentKanji,Image,SentAudio --word %GDWORD%
```

## gd-translate

**Usage**

```
gd-translate --sentence %GDSEARCH%
gd-translate --spoiler yes --to fr --sentence %GDSEARCH%
```

**Dependencies**

This script requires [Argos Translate](https://github.com/argosopentech/argos-translate) and the JA -> target language package to be installed.

```sh
pipx install argostranslate
argospm install translate-ja_en

argos-translate -f ja --t en "Hello World!"
```

If you can't install `argostranslate` due to an error, try an older version of python.

```sh
pipx install --python /usr/bin/python3.9 argostranslate
```

## gd-mandarin

This script passes a sentence through mecab in order to make every part of the sentence clickable.
It also automatically converts the sentence to traditional characters.

![image](https://github.com/KonstantinDjairo/gd-tools/assets/53496273/20921976-9221-416e-820a-b6bb22db906b)

To use `gd-mandarin`,
you need to install `gd-tools` by running `./quickinstall.sh --mandarin`.

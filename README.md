

## gd-mecab

This script passes a sentence through mecab in order to make every part of the sentence clickable

**Dependencies**

This script requires [MeCab](https://taku910.github.io/mecab/) and the IPA dictionary installed. \
If you are on an Arch Linux system you can simply install the AUR package `mecab-ipa` to obtain both.

**Arguments**

* `--font-size SIZE` the font size to be used, e.g. `30px`.
* `--user-dict FILE` full path to the user_dic.dic file. This is done automatically if you install via make.

**Tipps**

- The sentence is stored in the primary clipboard. So if you don't like the suggestions, you can search for some different substring. After that you can restore the sentence with a middle click.

## gd-pictures
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

This script shows 5 example sentences from https://massif.la/
![image](https://user-images.githubusercontent.com/50422430/226018360-e46605f0-2fb4-481c-801e-73aca84fae70.png)

## Installation
Run `sudo make install`.\
Open GoldenDict, press "Edit" > "Dictionaries" > "Programs" and add this script as type html and Command Line `<name of script> %GDWORD%`.\
Optionally add arguments, such as: `gd-mecab %GDWORD% --user-dict <path> --font-size 20px`.\
Now this program is treated as a dictionary and you can add it under Dictionaries or Groups.

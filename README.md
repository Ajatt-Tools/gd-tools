

## gd-mecab

This script passes a sentence through mecab in order to make every part of the sentence clickable

### Dependencies

This script requires [MeCab](https://taku910.github.io/mecab/) and the IPA dictionary installed. Both can be found on the AUR if you are on an Arch Linux system.

### Installation
Create a folder under ~/.goldendict/styles (name of your choosing) and place the provided css file there. Then enable the style under "Edit" > "Settings" > "Interface" > "Add-on style".\
Proceed with the [general installation instructions](#general-installation-instructions), but add `mecabgoldendict %GDWORD% </path/to/user_dic.dic>` for Command Line instead.

### Tipps
- The sentece is stored in the primary clipboard. So if you don't like the suggestions, you can search for some different substring. After that you can restore the sentence with a middle click.

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

## General installation instructions
Place the main script (file with same name no ending) in your PATH .\
Open GoldenDict, press "Edit" > "Dictionaries" > "Programs" and add this script as type html and Command Line `<insert name of script> %GDWORD%`.\
Now this program is treated as a dictionary and you can add it under Dictionaries or Groups.

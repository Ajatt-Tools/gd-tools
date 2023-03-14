

## gd-mecab

This script passes a sentence through mecab in order to make every part of the sentence clickable

### Dependencies

This script requires [MeCab](https://taku910.github.io/mecab/) and the IPA dictionary installed. Both can be found on the AUR if you are on an Arch Linux system.

## Installation
Create a folder under ~/.goldendict/styles (name of your choosing) and place the provided css file there. Then enable the style under "Edit" > "Settings" > "Interface" > "Add-on style".\
Proceed with the [general installation instructions](#general-installation-instructions)


## gd-pictures
This script shows the top 5 pictures from Brave Images for the given search string.

![image](https://user-images.githubusercontent.com/50422430/224538373-4311d13c-c650-4cb7-917b-18b4148a5efd.png)

### Dependencies
This script requires the python package beautifulsoup, which can be installed with `pip install beautifulsoup4`.

### Installation
Edit the script to point to the `gd-pic.py` file.
Proceed with the [general installation instructions](#general-installation-instructions)


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

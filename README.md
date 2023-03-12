

# gd-mecab
This script passes a sentence through mecab in order to make every part of the sentence clickable
## Dependencies
This script requires [MeCab](https://taku910.github.io/mecab/) and the IPA dictionary installed
## Installation
Add this script to your PATH.\
Open GoldenDict, press "Edit" > "Dictionaries" > "Programs" and add this script as type html and Command Line `mecabgoldendict %GDWORD%`. Now this program is treated as a dictionary and you can add it under Dictionaries or Groups.\
\
optionally create a folder under ~/.goldendict/styles (name of your choosing) and place the provided css file there. Then enable the style under "Edit" > "Settings" > "Interface" > "Add-on style"\
\
optionally compile and add furigana_link_split (https://github.com/GenjiFujimoto/mecab-tools) to your path and comment out the mecab implementation in `mecabgoldendict` to have furigana (though not working well yet). The provided CSS shows the furigana only on hover.

# gd-pictures
This script shows the top 5 pictures from google for the given search string.

![image](https://user-images.githubusercontent.com/50422430/224538373-4311d13c-c650-4cb7-917b-18b4148a5efd.png)

## Installation
Place the script `gd-pics` in your PATH and edit the script to point to the `gd-pic.py` file. You might have to need to install beautifulsoup, which can be done with
`pip install beautifulsoup4` for example.\
Open GoldenDict, press "Edit" > "Dictionaries" > "Programs" and add this script as type html and Command Line `gd-pics %GDWORD%`.

#!/usr/bin/env python

import requests
from bs4 import BeautifulSoup
import sys

search_term = sys.argv[1]
response = requests.get(f"https://www.bing.com/images/search?q={search_term}")
soup = BeautifulSoup(response.text, "html.parser")
image_tags = soup.find_all("img", class_="mimg")
# print(soup)

image_urls = []
for img in image_tags[:5]:
    src = img.get("src")
    if src:
        image_urls.append(f'<img src="{src}">')

html_output = f'<div id="golden-images"><section id="image-grid"><div class="image-list"> {" ".join(image_urls)} </div></section></div>'

print(html_output)

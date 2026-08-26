#!/usr/bin/env python3
import sys
import requests
from html import escape

def get_images(word, google_api_key, search_engine_id):
    """Fetch images from Google Custom Search API"""
    url = "https://www.googleapis.com/customsearch/v1"
    params = {
        'key': google_api_key,
        'cx': search_engine_id,
        'q': word,
        'searchType': 'image',
        'num': 10,
        'lr': 'lang_ja',
        'gl': 'jp',
        'cr': 'countryJP'
    }
    
    try:
        response = requests.get(url, params=params)
        response.raise_for_status()
        data = response.json()

        return data.get('items', [])
    except requests.exceptions.RequestException as e:
        sys.stderr.write(f"Error fetching images: {e}\n")
        return []

def generate_html(word, images):
    """Generate HTML output suitable for GoldenDict"""
    html = f"""
    <style>
        .image-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
            gap: 10px;
            padding: 10px;
        }}
        .image-card {{
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 5px;
            background: white;
        }}
        .image-card img {{
            width: 100%;
            height: 150px;
            object-fit: cover;
            border-radius: 2px;
        }}
        .image-title {{
            font-size: 12px;
            color: #666;
            margin-top: 5px;
            overflow: hidden;
            text-overflow: ellipsis;
            white-space: nowrap;
        }}
        .attribution {{
            font-size: 10px;
            color: #999;
            margin-top: 10px;
            text-align: center;
        }}
    </style>
    <div class="image-grid">
    """
    
    for image in images:
        title = escape(image.get('title', 'Image'))
        html += f"""
        <div class="image-card">
            <img src="{image['link']}" alt="{title}">
            <div class="image-title">{title}</div>
        </div>
        """
    
    html += """
    </div>
    <div class="attribution">
        Images from Google Custom Search
    </div> """
    
    return html

def main():
    if len(sys.argv) != 2:
        sys.stderr.write(f"Usage: python {sys.argv[0]} <word>\n")
        sys.exit(1)
    
    word = sys.argv[1]
    google_api_key = os.getenv('GOOGLE_API_KEY')
    search_engine_id = os.getenv('SEARCH_ENGINE_ID')
    images = get_images(word)
    html = generate_html(word, images)
    print(html)

if __name__ == "__main__":
    main()

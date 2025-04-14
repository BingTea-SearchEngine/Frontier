import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin, urlparse
import tldextract
import random
import time

START_URLS = ['https://en.wikipedia.org', 'https://www.bbc.com', 'https://www.reddit.com']
HEADERS = {'User-Agent': 'Mozilla/5.0 (compatible; RandomCrawler/1.0)'}
CRAWL_LIMIT = 10000
TIMEOUT = 5

def is_valid_url(url):
    try:
        result = urlparse(url)
        return result.scheme in ('http', 'https')
    except:
        return False

def extract_links(base_url, html):
    soup = BeautifulSoup(html, 'html.parser')
    links = set()
    for tag in soup.find_all('a', href=True):
        href = tag.get('href')
        full_url = urljoin(base_url, href)
        if is_valid_url(full_url):
            ext = tldextract.extract(full_url)
            if ext.domain and ext.suffix:  # skip mailto: and javascript:
                links.add(full_url)
    return links

def crawl():
    to_visit = set(START_URLS)
    visited = set()
    seed_list = []

    while len(seed_list) < CRAWL_LIMIT and to_visit:
        current_url = to_visit.pop()
        if current_url in visited:
            continue
        visited.add(current_url)

        try:
            response = requests.get(current_url, headers=HEADERS, timeout=TIMEOUT)
            if 'text/html' not in response.headers.get('Content-Type', ''):
                continue

            links = extract_links(current_url, response.text)
            new_links = list(links - visited)
            random.shuffle(new_links)
            to_visit.update(new_links[:50])  # Limit how many we add to avoid explosion

            for link in new_links:
                if len(seed_list) >= CRAWL_LIMIT:
                    break
                if link not in seed_list:
                    seed_list.append(link)

            print(f"Visited: {len(visited)} | Collected: {len(seed_list)}")

        except Exception as e:
            print(f"Error with {current_url}: {e}")

        time.sleep(random.uniform(0.5, 1.5))  # polite crawling

    return seed_list

if __name__ == '__main__':
    links = crawl()
    with open('seedList3.txt', 'w') as f:
        for url in links:
            f.write(url + '\n')
    print(f"Saved {len(links)} links to seed_list.txt")

import urllib.request
from bs4 import BeautifulSoup
import re, os

# Create folders to store website and text files
if not os.path.exists("./website_downloads/output"):
    os.makedirs("./website_downloads/output")

# Create master file to store all words found and top occurrences
if not os.path.exists("./website_downloads/output/master.txt"):
    open("./website_downloads/output/master.txt", "w")
if not os.path.exists("./website_downloads/output/occurrences.txt"): 
    open("./website_downloads/output/occurrences.txt", "w")

# List is used to store all wikipedia pages
pages = []

# For loop to create all letter combinations used for wikipedia pages
for first in range(65, 91):
    for second in range(65, 91):
        pages += [chr(first) + chr(second)]

for page in pages:
    # Download wikipedia page and read its content
    file = urllib.request.urlopen("https://en.wikipedia.org/wiki/" + page)
    str_html = file.read()
    # Parse html file for only words
    obj_html = BeautifulSoup(str_html, "html.parser")
    for element in obj_html(["script", "style"]):
        element.extract()
    # Stripped text from html file
    str_text = obj_html.get_text()
    regex = '[a-zA-Z][a-zA-Z_]*'
    # Open master text file
    with open("./website_downloads/output/master.txt", "a+") as master:
        # Write all regex words to master file
        for match in re.finditer(regex, str_text):
            s = match.group()
            master.write(s + "\n")

# Read all words in master file and count in dictionary
word_count = {}
with open("./website_downloads/output/master.txt", "r") as master:
    lines = master.readlines()
    for word in lines:
        lowercase = word.lower()
        if lowercase in word_count:
            word_count[lowercase] += 1
        else:
            word_count[lowercase] = 1

# Sort dictionary in reverse order and print top 15
with open("./website_downloads/output/occurrences.txt", "w") as occurences:
    length = 0
    for (word, count) in sorted(word_count.items(), key=lambda item: item[1], reverse=True):
        occurences.write(str(count) + " " + word)
        length += 1
        if (length == 15):
            break
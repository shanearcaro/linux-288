#!/bin/bash

# Create folders to store website and text files
mkdir -p "./website_downloads/pages"
mkdir -p "./website_downloads/text"
mkdir -p "./website_downloads/output"

# Create master file to store all words found and top occurrences
touch "./website_downloads/output/master.txt"
touch "./website_downloads/output/occurrences.txt"

# Function I found online to convert numbers to ascii
chr() {
  [ "$1" -lt 256 ] || return 1
  printf "\\$(printf '%03o' "$1")"
}

# Array is used to store all wikepedia pages
pages=()

# For loop to create all letter combinations used for wikipedia pages
for ((i=65; $i<=90; i=$i+1)); do
    for ((j=65; $j<=90; j=$j+1)); do
        pages+=("$(chr $i)$(chr $j)")
    done
done

pages_length=${#pages[*]}

# Download all wikipedia pages and convert to text file
for ((i=0; $i<$pages_length; i=$i+1)); do
    wget "https://en.wikipedia.org/wiki/${pages[$i]}" -O "./website_downloads/pages/${pages[$i]}.html"
    lynx -dump -nolist "./website_downloads/pages/${pages[$i]}.html" > "./website_downloads/text/${pages[$i]}.txt"
    # Once file is downloaded add all words to master file with one word per line
    echo $(cat "./website_downloads/text/${pages[$i]}.txt" | grep -wo -E '[a-zA-Z][a-zA-Z_]*' | sort -f) | tr " " "\n" >> "./website_downloads/output/master.txt" 
done

# Sort all occurences of words so uniq can count then resort in reverse number order and print the top 15
# Split file so only two words can fit on a line to make output more readable
echo $(cat "./website_downloads/output/master.txt" | sort | uniq -ic | sort -nr | head -15) | xargs -n2 > "./website_downloads/output/occurrences.txt"
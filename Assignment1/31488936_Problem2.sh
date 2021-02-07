#/bin/bash

# Read file
echo -n "Enter a file name: "
read file

# If file exists copy with .bak and current time otherwise print error
if [ -f $file ]; then
    cp $file "$file.bak$(date +%R)"
else
    echo "$file is not a file"
fi
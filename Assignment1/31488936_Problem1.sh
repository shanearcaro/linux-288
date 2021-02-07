#!/bin/bash

# Read file location 
echo -n "Enter path of a file or directory: "
read location

# If file path doesn't exist exit the program
if [ ! -e $location ]; then
    echo "Path does not exist."
    exit 1
fi

# Check path type and print to user
echo -n "Path type: "
if [ -f $location ]; then
    echo "file"
else
    echo "directory"
fi

# Check permissions and print to user
echo -n "Permissions: "
if [ -r $location ]; then
    echo -n "read "
fi
if [ -w $location ]; then
    echo -n "write "
fi
if [ -x $location ]; then
    echo -n "execute"
fi
echo ""

# If folder, print out size and category
if [ -f $location ]; then
    size=$(du -b $location | cut -f1)
    echo "File size: " $size
    echo -n "File category: "
    if [ $size -gt 1048576 ]; then
        echo "Large"
    elif [ $size -le 1048576 ] && [ $size -gt 102400 ]; then
        echo "Medium"
    else
        echo "Small"
    fi
fi

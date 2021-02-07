#/bin/bash

# Check for specified directory otherwise use current file location
directory="."
if [ $# -eq 1 ]; then
    directory=$1
fi
echo "Searching in: $directory"
echo ""

# Recursive function to search directories
recursive_search() {
    directory=$1
    content=($(ls $directory))

    # Get length of the array
    length=${#content[*]}

    # Add beginning path to subpath
    for ((i=0; $i<$length;i=$i+1)); do
        content[$i]="$directory/${content[$i]}"
    done

    # Empty array of directories to recursively search
    folders=()

    # Loop elements in directory
    for ((i=0; $i<$length;i=$i+1)); do
        # Check if path is a file or a direcotry
        if [ -f ${content[$i]} ]; then
            # Path is file
            echo "File found: ${content[$i]}"

            # Check size of file
            size=$(du -b ${content[$i]} | cut -f1)

            # If file size is 0 delete the file, otherwise pass
            if [ $size -eq 0 ]; then
                echo "Deleting: ${content[$i]}"
                rm ${content[$i]}
            else
                echo "Passing: ${content[$i]}"
            fi

        else
            # Path is directory
            echo "Directory found: ${content[$i]}"
            # Add directory to folders array to search later
            folders+=(${content[$i]})
        fi
        echo ""
    done

    # Iterate through folders array recursively
    for folder in ${folders[*]}; do
        echo "Searching Deeper: $folder"
        recursive_search $folder
    done
}

recursive_search "$directory"

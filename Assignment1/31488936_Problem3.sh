#/bin/bash

# Set sum to integer value of 0
sum=$((0))

# Loop through arugments and sum them
for number in $*; do
    sum=$(($sum+number))
done
echo "Total: $sum"
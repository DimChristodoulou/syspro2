#!/bin/bash
#./myscript.sh -e conf -s /etc -l /usr/lib /etc/hosts

clientArray=()
bytes=0
#IFS=' '
while read -ra var
do
    for i in "${!var[@]}";
    do # access each element of array
        if [ $i -eq 1 ]
        then
            clientArray+=("${var[$i]}")
            #echo "${var[$i]}"
        fi

        if [ $i -eq 13 ]
        then            
            bytes=$(($bytes+${var[$i]}))
        fi
    done
done
uniq=($(printf "%s\n" "${clientArray[@]}" | sort -u | tr '\n' ' '))
echo "List of clientIDs involved in the process ${uniq[*]}"

max=${uniq[0]}
min=${uniq[0]}
for i in "${uniq[@]}"; do
  (( i > max )) && max=$i
  (( i < min )) && min=$i
done

echo "Max clientID is $max"
echo "Min clientID is $min"

echo "Total bytes sent $bytes"
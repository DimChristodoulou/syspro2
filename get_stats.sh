#!/bin/bash
#./myscript.sh -e conf -s /etc -l /usr/lib /etc/hosts

clientArray=()
dissapearedClients=()
bytes=0
count=0
#IFS=' '
re='^[0-9]+$'
while read -ra var
do    
    if [[ ${var[4]} == *"file"* ]];
    then
        count=$(($count+1))
    fi

    if [[ ${var[3]} == *"DISSAPEARED"* ]];
    then        
        dissapearedClients+=("${var[1]}")
    fi

    for i in "${!var[@]}";
    do # access each element of array
        if [ $i -eq 1 ]
        then
            if [[ ${var[$i]} =~ $re ]] ; then
                clientArray+=("${var[$i]}")
            fi            
            #echo "${var[$i]}"
        fi

        if [ $i -eq 13 ]
        then            
            bytes=$(($bytes+${var[$i]}))
        fi
    done
done
uniq=($(printf "%s\n" "${clientArray[@]}" | sort -u | tr '\n' ' '))
echo "List of clientIDs involved in the process: ${uniq[*]}"

max=${uniq[0]}
min=${uniq[0]}
for i in "${uniq[@]}"; do
  (( i > max )) && max=$i
  (( i < min )) && min=$i
done

echo "Max clientID is $max"
echo "Min clientID is $min"

echo "Total bytes sent $bytes"
echo "Total files sent $count"

uniqClients=($(printf "%s\n" "${dissapearedClients[@]}" | sort -u | tr '\n' ' '))
echo "List of clientIDs that left the system: ${uniqClients[*]}"
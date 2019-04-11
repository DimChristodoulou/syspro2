#!/bin/bash
#./myscript.sh -e conf -s /etc -l /usr/lib /etc/hosts

if [ $# -ne 4 ]
then
    echo Wrong number of arguments
    exit 0
else
    dirName="$1"
    numOfFiles="$2"
    numOfDirs="$3"
    levels="$4"
    echo "$dirName $numOfFiles $numOfDirs $levels"
fi

robin=$(($numOfFiles/$numOfDirs))
robinMod=$(($numOfFiles%$numOfDirs))
if [ $robin -eq 0 ]
then
    robin=1
fi

if [ ! -d "$dirName" ];
then
  # $dirName does not exist.
  echo Directory does not exist
  mkdir -p "$dirName"
fi
cd "$dirName"

currNumOfDirs=0
currNumOfFiles=0

echo $robin
echo $robinMod

while [ $currNumOfDirs -ne $numOfDirs ]
do
    for (( i=1; i<=$levels; i++ ))
    do
        folderNameSize=$(( ( RANDOM % 8 )  + 1 ))
        folderName=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $folderNameSize | head -n 1)
        mkdir "$folderName"
	    cd "$folderName"
        if [ $currNumOfFiles -ne $numOfFiles ]
        then
            for (( j=1; j<=$robin; j++ ))
            do
                fileNameSize=$(( ( RANDOM % 8 )  + 1 ))
                textSize=$(( ( RANDOM % 1024 )  + 1 ))
                fileName=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $fileNameSize | head -n 1)
                fileContents=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $textSize | head -n 1)
                echo $fileContents >> $fileName
                currNumOfFiles=$(($currNumOfFiles+1))
            done
        fi
        currNumOfDirs=$(($currNumOfDirs+1))
        if [ $currNumOfDirs -eq $numOfDirs ]
        then
            break
        fi
    done
    for (( i=1; i<=$levels; i++ ))
    do
        cd ".."
    done
done
cat 0a10aced202194944a004c08.log| grep "\[app]"|grep -v "WARN" |awk -F' ' '{ print $1 "T" $2 "," $6 "," $9 }'|sed -e "s/\[//g"|sed -e "s/]//g" > 0a10aced202194944a004c08.csv

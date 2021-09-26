#!/bin/bash

# get data
wget https://github.com/OSMNames/OSMNames/releases/download/v2.1.1/planet-latest_geonames.tsv.gz

# extract belgium
zcat planet-latest_geonames.tsv.gz |
    awk -F $'\t' 'BEGIN {OFS = FS} {if(NR!=1){if($16 =="be"){print}}else{print}}' |
    gzip - >countryExtract.tsv.gz

# split double names, add alt names (+ ignore errors in Brussel)
zcat countryExtract.tsv.gz |
    awk -F $'\t' 'BEGIN{OFS="\t"} {n=split($1,a,";");for(i=1;i<=n;i++){print($4,a[i],$9,$7,$8)}} $9 > 16 && $2 != "\"\"" && $2 ~ /^[A-Za-z ]*$/ {print($4,$2,$9,$7,$8)}' >belgie_full.data

rm countryExtract.tsv.gz
rm planet-latest_geonames.tsv.gz
sed -i '1d' belgie_full.data
# shellcheck disable=SC2196
egrep "^[0-9]+\s[^\s]+\s[^\s]+\s[^\s]+\s[^\s]+$" belgie_full.data >belgie.data # Plaatsen zonder plaatsnaam wegfilteren
rm belgie_full.data

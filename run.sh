#!/bin/bash

HAMMER_BIN=src/hammertest64

mkdir -p results

mfg=$(sudo dmidecode | grep "Handle 0x0010" -A 19 | grep Manufacturer | cut -d ':' -f 2 | tr -d ' \n')
serial=$(sudo dmidecode | grep "Handle 0x0010" -A 19 | grep "Serial Number" | cut -d ':' -f 2 | tr -d ' \n')

name=results/$mfg$serial
i=1
if [[ -e $name.$i ]] ; then
    while [[ -e $name.$i ]] ; do
        let i++
    done
fi

name=$name.$i

sudo dmidecode | grep "Handle 0x0010" -A 19 | tee $name
sudo ./$HAMMER_BIN | tee -a $name

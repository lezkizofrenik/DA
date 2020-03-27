#!/bin/bash
niveles=(3124 3125 3054 3056 3056 3056 3052 3028 4056 4092)
pases=(0.5 0.6 0.4 0.5 0.7 1 0.5 0.6 0.3 0.5)
defensas=(30 40 40 50 40 50 40 60 70 80)
defensestypes=(30 40 30 40 20 50 40 50 70 70)
randomness=(0.5 0.6 0.4 0.5 0.7 0.5 0.5 0.6 0.3 0.5)

for i in "${!niveles[@]}"; do
     echo "Nivel: ${niveles[$i]}"
    ../simulador/simulador -level "${niveles[$i]}" -pases "${pases[i]}" -defenses "${defensas[$i]}" -defensetypes "${defensestypes[$i]}" -dr "${randomness[$i]}" $i 
done

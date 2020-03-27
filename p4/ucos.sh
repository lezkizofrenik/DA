#!/bin/bash
niveles=(1224 2225 2054 2056 2556 2452 2428 3256 3292)

for i in "${niveles[@]}"; do
     echo "Nivel: $i"
    ../simulador/simulador -level $i
done

#!/bin/bash

for i in traces/*; do
	if [ -d "$i" ]; then
		trace=$(basename "$i")
	 	MPKI=$(./run $i | grep -oP '(?<=MPKI: )[0-9]+([.][0-9]+)')
	 	printf "%s : %s\n" "$trace" "$MPKI"
	fi
done

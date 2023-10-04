#!/bin/bash
cat ips.txt | xargs -n 1 -I {} ssh "{}" $@

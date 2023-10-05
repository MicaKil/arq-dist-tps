#!/bin/bash
# shellcheck disable=SC2068
# shellcheck disable=SC2002
cat ips.txt | xargs -n 1 -I {} ssh "{}" $@

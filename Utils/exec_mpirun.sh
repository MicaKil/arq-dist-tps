#!/bin/bash
# shellcheck disable=SC2068
# shellcheck disable=SC2068
mpirun --hostfile ips.txt --mca btl_tcp_if_exclude docker0,127.0.0.1/8 $@

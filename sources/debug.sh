#!/bin/bash
# RUN UNDER SUDO

killall -q apache2

ORIGINAL_HOME=${HOME}
source /etc/apache2/envvars
export HOME=${ORIGINAL_HOME}

cgdb --args /usr/sbin/apache2 -X

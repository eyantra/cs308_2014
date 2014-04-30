#!/bin/bash

RESTART_DELAY=0.5
SERIALMON_PATH=`cd $(dirname $0) ; pwd -P`
SERIALMON_OUT_PATH="/var/log/serialmon"

rm -rf "$SERIALMON_OUT_PATH"
mkdir -p "$SERIALMON_OUT_PATH"

function on_exit {
    kill 0
    mv "$SERIALMON_OUT_PATH/temp.dat" "$SERIALMON_OUT_PATH/serialmon.dat"
}
trap on_exit EXIT

until "$SERIALMON_PATH/serialmon.sh"; do
    sleep $RESTART_DELAY
done

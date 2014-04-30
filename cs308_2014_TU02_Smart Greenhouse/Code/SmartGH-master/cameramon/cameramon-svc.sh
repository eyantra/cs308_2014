#!/bin/bash

RESTART_DELAY=0.5
CAMERAMON_OUT_PATH="/var/log/cameramon"

rm -rf "$CAMERAMON_OUT_PATH"
mkdir -p "$CAMERAMON_OUT_PATH"

function on_exit {
    kill 0
}
trap on_exit EXIT

while :; do
    nice -5 fswebcam --resolution 640x480 --skip 1 --loop 1 --quiet \
                     --banner-colour "#FF000000" --line-colour "#FF000000" \
                     --top-banner --font DroidSans-Bold:13 --no-shadow \
                     --title "CAM 0" --timestamp "%a %d-%m %H:%M:%S" \
                     --save "$CAMERAMON_OUT_PATH/camera.dat"
    sleep $RESTART_DELAY
done

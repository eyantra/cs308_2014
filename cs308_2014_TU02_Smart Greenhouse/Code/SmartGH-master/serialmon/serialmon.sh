#!/bin/bash

DELAY=1.8
MAX_HISTORY=3600
MAX_EVENTS=1024

GPIO=/usr/local/bin/gpio

SERIALMON_PATH=`cd $(dirname $0) ; pwd -P`
SERIALMON_OUT_PATH="/var/log/serialmon"

THRESHOLDS_PATH="/var/opt/serialmon/thresholds"

log_and_trim () {
    echo -e `date +"%s"` "\t$1" >> "$SERIALMON_OUT_PATH/$2"
    echo -e `date +"%s"` "\t$1"
    tail -n $3 "$SERIALMON_OUT_PATH/$2" > "$SERIALMON_OUT_PATH/temp.dat"
    mv "$SERIALMON_OUT_PATH/temp.dat" "$SERIALMON_OUT_PATH/$2"
}

while :; do
    readings=`awk -f "$SERIALMON_PATH/read_serial_ama0.awk" /dev/ttyAMA0`
    log_and_trim "$readings" "serialmon.dat" $MAX_HISTORY

    T0=`echo -e "$readings" | cut -f 1`
    L0=`echo -e "$readings" | cut -f 2`
    RH0=`echo -e "$readings" | cut -f 3`
    T1=`echo -e "$readings" | cut -f 4`
    L1=`echo -e "$readings" | cut -f 5`
    RH1=`echo -e "$readings" | cut -f 6`

    threshL=`cat $THRESHOLDS_PATH/L`
    threshT=`cat $THRESHOLDS_PATH/T`
    threshRH=`cat $THRESHOLDS_PATH/RH`

    let "avgT=($T0+$T1)/2"
    let "avgL=($L0+$L1)/2"
    let "avgRH=($RH0+$RH1)/2"

     if [ "$avgL" -lt "$threshL" ]; then
        if [ `$GPIO read 4` -eq 0 ]; then
            log_and_trim "1\tLight Intensity" "events.dat" $MAX_EVENTS
            $GPIO write 4 1
        fi
    else
        if [ `$GPIO read 4` -eq 1 ]; then
            log_and_trim "0\tLight Intensity" "events.dat" $MAX_EVENTS
            $GPIO write 4 0
        fi
    fi

    if [ "$avgT" -gt "$threshT" ]; then
        if [ `$GPIO read 3` -eq 0 ]; then
            log_and_trim "1\tTemperature" "events.dat" $MAX_EVENTS
            $GPIO write 3 1
        fi
    else
        if [ `$GPIO read 3` -eq 1 ]; then
            log_and_trim "0\tTemperature" "events.dat" $MAX_EVENTS
            $GPIO write 3 0
        fi
    fi

    if [ "$avgRH" -lt "$threshRH" ]; then
        if [ `$GPIO read 2` -eq 0 ]; then
            log_and_trim "1\tHumidity" "events.dat" $MAX_EVENTS
            $GPIO write 2 1
        fi
    else
        if [ `$GPIO read 2` -eq 1 ]; then
            log_and_trim "0\tHumidity" "events.dat" $MAX_EVENTS
            $GPIO write 2 0
        fi
    fi

    if [ `$GPIO read 2` -eq 1 ] || [ `$GPIO read 3` -eq 1 ]; then
        $GPIO write 1 1
    else
        $GPIO write 1 0
    fi

    sleep $DELAY
done

#!/bin/bash

install () {
    apt-get install fswebcam
    
    OWN_PATH=`cd $(dirname $0) ; pwd -P`

    cd /opt
    ln -s $OWN_PATH/cameramon cameramon
    ln -s $OWN_PATH/serialmon serialmon

    cd /etc/init.d
    ln -s /opt/cameramon/cameramon-service.sh cameramon
    ln -s /opt/serialmon/serialmon-service.sh serialmon

    update-rc.d cameramon defaults
    update-rc.d serialmon defaults

    mkdir -p /var/opt/serialmon
    cp -R $OWN_PATH/default-thresholds /var/opt/serialmon/thresholds
    chmod -R 777 /var/opt/serialmon/thresholds

    service cameramon start
    service serialmon start
}

uninstall () {
    service cameramon stop
    service serialmon stop

    rm -rf /var/opt/serialmon/thresholds

    update-rc.d cameramon remove
    update-rc.d serialmon remove

    rm /etc/init.d/cameramon
    rm /etc/init.d/serialmon
    
    rm -rf /opt/cameramon
    rm -rf /opt/serialmon
}

case "$1" in

install)
    install
    ;;

uninstall)
    uninstall
    ;;

*)
    echo "Invalid argument -- $1"
    ;;
esac

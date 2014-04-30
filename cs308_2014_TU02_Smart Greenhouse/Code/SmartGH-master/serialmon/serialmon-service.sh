#!/bin/bash

### BEGIN INIT INFO
# Provides:		serialmon-service
# Required-Start:	$remote_fs $syslog
# Required-Stop:	$remote_fs $syslog
# Default-Start:	2 3 4 5
# Default-Stop:		0 1 6
# Short Description:	Captures ttyAMA0 lines
# Long Description:	Captures tyyAMA0 lines
### END INIT INFO

DIR=/opt/serialmon
DAEMON=$DIR/serialmon-svc.sh
DAEMON_NAME=serialmon-service
DAEMON_USER=root

PID_FILE=/var/run/$DAEMON_NAME.pid

. /lib/lsb/init-functions

do_start () {
    log_daemon_msg "[+] Starting system $DAEMON_NAME daemon"
    start-stop-daemon --start --background --pidfile $PID_FILE --make-pidfile \
                      --user $DAEMON_USER --chuid $DAEMON_USER --startas $DAEMON \
                      --nicelevel -5 --iosched real-time --quiet 2>/dev/null
    log_end_msg $?
}

do_stop () {
    log_daemon_msg "[-] Stopping system $DAEMON_NAME daemon"
    start-stop-daemon --stop --pidfile $PID_FILE --retry 5 --quiet 2>/dev/null
    log_end_msg $?
}

case "$1" in
start|stop)
    do_${1}
    ;;
restart|reload|condrestart|force-reload)
    do_stop
    do_start
    ;;
status)
    status_of_proc "$DAEMON_NAME" "$DAEMON" && exit 0 || exit $?
    ;;
*)
    echo "Usage: $0 {start|stop|status|restart}"
    exit 1
    ;;
esac
exit 0

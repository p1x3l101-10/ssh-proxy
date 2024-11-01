#!/usr/bin/env bash
_SSH_HOST="borg@166.113.94.161 -p 7003"

NAME="ssh-proxy"
PIDFILE="/tmp/$NAME/$NAME.pid"
LOGFILE="/tmp/$NAME/$NAME.log"
ERRFILE="/tmp/$NAME/$NAME.err"
RUNCOMMAND="ssh -vvv -D 0.0.0.0:9050 -C -N $_SSH_HOST"


function stopDaemon () {
  case $(daemonStatus) in
    'running')
      kill $(cat $PIDFILE)
      rm $PIDFILE
      launchctl unsetenv all_proxy
      ;;
    'degraded')
      echo "Service did not exit cleanly, manual clean-up required."
      echo "Such actions include: unsetting variables and removing stray files"
      ;;
    'stopped')
      echo"Service already stopped"
      ;;
  esac
}

function startDaemon () {
  eval $RUNCOMMAND &
  local dPID="$!"
  echo "$dPID" > $PIDFILE
  launchctl setenv all_proxy 'socks5h://127.0.0.1:9050'
  wait $dPID
}

daemonStatus () {
  if [[ -f $PIDFILE ]]; then
    if ps -p $(cat $PIDFILE) > /dev/null; then
      echo "running"
    else
      echo "degraded"
    fi
  else
    echo "stopped"
  fi
}

function daemonDirs () {
  mkdir -p $(dirname $PIDFILE)
  mkdir -p $(dirname $LOGFILE)
  mkdir -p $(dirname $ERRFILE)
}

case $1 in
  'DAEMON')
    shift
    startDaemon $*
    # Exit with code 1 if daemon fails
    exit 1
    ;;
  'start')
    echo "starting..."
    daemonDirs
    nohup $0 DAEMON $* 2> "$ERRFILE" > "$LOGFILE" &
    exit 0
    ;;
  'stop')
    echo "stopping..."
    stopDaemon
    exit 0
    ;;
  'restart')
    if ! [[ $(daemonStatus) == "running" ]]; then
      echo "Daemon not running."
      exit 1
    fi
    echo "restarting..."
    stopDaemon
    daemonDirs
    nohup $0 DAEMON $* 2> "$ERRFILE" > "$LOGFILE" &
    exit 0
    ;;
  'status')
    daemonStatus
    exit 0
    ;;
esac
echo "Command not recoginzed"
exit 1

# Build:
gcc -o inspirate_daemon daemon.c -L. -lapue

# Run:
sudo ./inspirate_daemon

# Show all daemons:
ps -ef | awk '$3 == 1'

# Print inspirate_daemon pid:
cat /var/run/inspirate_daemon.pid

# Show inspirate_daemon logs:
cat /var/log/syslog | grep inspirate_daemon

# Kill daemon (Send a SIGKILL signal):
sudo kill -9 $DPID

# Print daemon:
ps -ajx | head -1 && ps -ajx | grep pid

# Send a SIGHUP signal:
sudo kill -1 $DPID

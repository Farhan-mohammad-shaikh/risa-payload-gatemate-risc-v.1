# SIP Example

The example creates a coordinator and a worker that can either be used as counterparts for an existing Serial link or can be connected through virtual ports, for instance created by:

``socat -d -d pty,raw,echo=0 pty,raw,echo=0``

Build the coordinator with:

``make build-coordinator``

And the worker with:

``make build-worker``

For running you need to set the serial port as well as baud rate and transmission mode or leave them to the defaults (/dev/tty/S0, 115200 baud, no parity):

``./bin/sip-coordinator -p /dev/pts/4 -b 115200 -r n``

In another terminal you can run the corresponding worker:

``./bin/sip-worker -p /dev/pts/5 -b 115200 -r n``


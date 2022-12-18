#!/bin/bash
#
#  In case your platform uses different commands, flags or libraries,
#  you can adjust the following settings
#
CC=gcc
#
CFLAGS="-Wall"
#
LIBS="-lrt -lm"
#
#  To improve debugging with GDB, if compiled with gcc:
#CFLAGS="-Wall -ggdb2"
#
#  Useful commands for analyzing active applications:
#  gdb : debugger
#      gdb application
#            Start application within the gdb debug, in case the application fails.
#      gdb application pid
#            Connect gdb to the running application with the given process id pid.
#            Useful to check why or where the application is stuck.
#  strace : trace system calls
#  ltrace : trace library calls
#  valgrind : debugging memory and profiling
#  gprof : call graph execution profiler
#
echo router_dealer
$CC $CFLAGS -o router_dealer router_dealer.c $LIBS
echo client
$CC $CFLAGS -o client client.c request.c $LIBS
echo worker_s1
$CC $CFLAGS -o worker_s1 worker_s1.c service1.c $LIBS
echo worker_s2
$CC $CFLAGS -o worker_s2 worker_s2.c service2.c $LIBS

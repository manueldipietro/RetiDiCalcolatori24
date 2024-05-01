#!/bin/bash
clear 
gcc \
ADT/adt_utility.c \
ADT/iterator.c \
ADT/scl_ordered.c \
ADT/scl_ts.c \
ADT/scl.c \
ADT/avl.c \
\
main.c \
server.c \
client.c \
command.c \
expirer_manager.c \
\
tdbms_record.c \
tdbms.c \
\
utility.c \
timespec.c \
\
resp2/resp2_buffer.c \
resp2/resp2_string.c \
resp2/resp2_error.c \
resp2/resp2_number.c \
resp2/resp2_bulkstring.c \
resp2/resp2_array.c \
\
resp2/resp2_generics.c \
resp2/resp2_tcp_socket.c \
resp2/resp2_utility.c \
resp2/resp2_parser.c \
\
-o main
echo "Compilazione effettuata!"
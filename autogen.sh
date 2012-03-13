#!/bin/bash

autoreconf -f -i

if [ -z "$NOCONFIGURE" ]; then
    ./configure "$@"
fi


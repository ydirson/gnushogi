#!/bin/bash

autoreconf -f -i
# generate files that cannot be made in a cross-build setup yet
./configure
make -C gnushogi gnushogi_bootstrap
make distclean

echo "You may now run ./configure"

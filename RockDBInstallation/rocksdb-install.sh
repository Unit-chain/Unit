#!/bin/bash

sudo apt-get install -y libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev liblz4-dev libzstd-dev

pushd /tmp
git clone https://github.com/facebook/rocksdb.git && cd rocksdb

# Checkout to the specific version
git checkout v5.11.3

# Ignore GCC warnings
export CXXFLAGS='-Wno-error=deprecated-copy -Wno-error=pessimizing-move -Wno-error=class-memaccess'

# Build as a shared library
make shared_lib

# The following command installs the shared library in /usr/lib/ and the header files in /usr/include/rocksdb/:
make install-shared INSTALL_PATH=/usr

popd

# cleanup
rm -rf /tmp/rocksdb

# To uninstall
#make uninstall INSTALL_PATH=/usr
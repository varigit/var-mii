#!/bin/bash

set -e
readonly DIR=`realpath "${0%/*}"`
readonly REMOTE=varigit

usage() {
    echo 'Usage:'
    echo '  $ export SDK_32=<path to 32 bit sdk>'
    echo '  $ export SDK_64=<path to 64 bit sdk>'
    echo '  $ ./release.sh'
    echo
    echo 'Example:'
    echo '  $ export SDK_32="/opt/fslc-x11/3.1/environment-setup-cortexa7t2hf-neon-fslc-linux-gnueabi"'
    echo '  $ export SDK_64="/opt/fslc-xwayland/4.0/environment-setup-armv8a-fslc-linux"'
    echo '  $ ./release.sh'
    exit 1
}

do_cleanall() {
    cd ${DIR}/../
    make cleanall
}

do_build() {
    OUTPUT="$1"
    cd ${DIR}/../
    make clean
    make -j $(nproc)
    mv var-mii ${OUTPUT}
}

if [ -z "${SDK_32}" ]; then
    echo "Error: Please export SDK_32"
    usage
fi

if [ -z "${SDK_64}" ]; then
    echo "Error: Please export SDK_64"
    usage
fi

if [ ! -f "${SDK_32}" ]; then
    echo "Error: SDK_32 \"${SDK_32}\" missing"
    usage
fi

if [ ! -f "${SDK_64}" ]; then
    echo "Error: SDK_64 \"${SDK_64}\" missing"
    usage
fi

do_cleanall

source ${SDK_32}
do_build "var-mii-32"

source ${SDK_64}
do_build "var-mii-64"

if ! command -v gh &> /dev/null; then
    echo "gh could not be found"
    echo "Please install https://cli.github.com/"
    echo "You will need to Setup an SSH Key and link to account"
    exit 1
fi

COMMITID="$(git rev-parse --short HEAD)"

echo ${COMMITID}

git push ${REMOTE}
gh release create ${COMMITID} -t "var-mii@${COMMITID}" ${DIR}/../var-mii-32 ${DIR}/../var-mii-64

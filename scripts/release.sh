#!/bin/bash

set -e
readonly DIR=`realpath "${0%/*}"`
readonly REMOTE=varigit

if ! command -v gh &> /dev/null; then
    echo "gh could not be found"
    echo "Please install https://cli.github.com/"
    echo "You will need to Setup an SSH Key and link to account"
    exit 1
fi

COMMITID="$(git rev-parse --short HEAD)"

echo ${COMMITID}

git push ${REMOTE}
gh release create ${COMMITID} -t "var-mii@${COMMITID}" ${DIR}/../var-mii

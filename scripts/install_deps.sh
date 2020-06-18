#!/bin/bash
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	sudo apt-get update
	sudo apt-get install apt
	sudo apt-get clean
	sudo rm -rf /var/lib/apt/lists
	sudo apt-get update
    sudo apt-get install -qq qt5-default qttools5-dev-tools p7zip-full
    sudo apt-get update -qq
fi

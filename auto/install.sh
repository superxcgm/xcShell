#!/bin/sh

set -e

./auto/build.sh

sudo cp build/xcShell /usr/local/bin

if [ ! -f "/etc/xcshrc" ] ;then
  echo "Global config do not exist, auto create from default config."
  sudo cp auto/global_config_default.txt /etc/xcshrc
fi
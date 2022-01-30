#!/bin/sh

set -e

./auto/build.sh

sudo make install

if [ ! -f "/etc/xcshrc" ]; then
  echo "Global config do not exist, auto create from default config."
  sudo cp auto/data/global_config_default.txt /etc/xcshrc
fi

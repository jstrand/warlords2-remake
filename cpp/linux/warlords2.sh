#!/bin/sh
# Starts the game from its own folder, so that original/ and
# pre-rendered-sound/ are found. Arguments go through to the game.
cd "$(dirname "$(readlink -f "$0")")" || exit 1
exec ./warlords2 "$@"

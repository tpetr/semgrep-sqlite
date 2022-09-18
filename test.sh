#!/bin/bash -eu
gcc -g -fPIC -l semgrep_bridge_core -dynamiclib semgrep.c -o semgrep.dylib -v
./sqlite3 test.db "SELECT load_extension('./semgrep'); $@"
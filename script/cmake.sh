#!/bin/bash

PROJECT_NAME=$(basename -s .git `git config --get remote.origin.url`)

SCRIPT_PATH="$(dirname "$(readlink -f $0)")"
PROJECT_PATH="$(dirname "$SCRIPT_PATH")"
CMAKE_IN=$SCRIPT_PATH/CMakeLists.txt.in

PROJECT_KEY='{LOWER_PROJECT_NAME}'
PROJECT_VAL=$(echo "$PROJECT_NAME" | tr '[:upper:]' '[:lower:]')

sed "s/$PROJECT_KEY/$PROJECT_VAL/g" $CMAKE_IN > "$PROJECT_PATH/CMakeLists.txt"

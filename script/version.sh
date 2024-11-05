#!/bin/bash

# Get project name from git
PROJECT_NAME=$(basename -s .git `git config --get remote.origin.url`)

SCRIPT_PATH="$(dirname "$(readlink -f $0)")"
PROJECT_PATH="$(dirname "$SCRIPT_PATH")"
HDR_IN=$SCRIPT_PATH/version.h.in
HDR_HOME=$PROJECT_PATH/include/$PROJECT_NAME
HDR_PATH=$HDR_HOME/version.h
CPP_IN=$SCRIPT_PATH/version.cpp.in
CPP_HOME=$PROJECT_PATH/src/$PROJECT_NAME
CPP_PATH=$CPP_HOME/version.cpp

PROJECT_KEY='{PROJECT}'
PROJECT_VAL=${PROJECT_NAME}
NAMESPACE_KEY='{NAMESPACE}'
NAMESPACE_VAL=${PROJECT_NAME//-/_}
HASH_KEY='{HASH}'
HASH_VAL=`git rev-parse --short HEAD`
UPDATED_KEY='{UPDATED}'
UPDATED_VAL=`git log -1 --date=format:"%Y/%m/%d %T" --pretty=format:%ad`
UPDATED_CHANGE=$(echo $UPDATED_VAL $i | sed -e 's/\//\\\//g')

# Generate version.h file
if [ ! -d "$HDR_HOME/version.h" ]; then
	mkdir -p $HDR_HOME
fi
sed "s/$PROJECT_KEY/$PROJECT_VAL/g" $HDR_IN | sed "s/$NAMESPACE_KEY/$NAMESPACE_VAL/g" > $HDR_PATH 

# Generate version.cpp file
if [ -d $CPP_PATH ]; then
	rm $CPP_PATH
fi
if [ ! -d $CPP_HOME ]; then
	mkdir -p $CPP_HOME
fi
sed "s/$HASH_KEY/$HASH_VAL/g" $CPP_IN | sed "s/$UPDATED_KEY/$UPDATED_CHANGE/g" | sed "s/$PROJECT_KEY/$PROJECT_VAL/g" | sed "s/$NAMESPACE_KEY/$NAMESPACE_VAL/g" > $CPP_PATH 

echo update revision $HASH_VAL "($UPDATED_VAL)"

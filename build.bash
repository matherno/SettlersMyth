#!/bin/bash

if [ "$1" = "" ]; then
    premake5 gmake && make SettlersMyth
fi

if [ "$1" = "debug" ]; then
    premake5 gmake && make config=debug SettlersMyth
fi

if [ "$1" = "release" ]; then
    premake5 gmake && make config=release SettlersMyth
fi

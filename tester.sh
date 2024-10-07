#!/usr/bin/env bash

# This script is used to test the program

TESTS_PATH="./tests"
SOLUTION_PATH="./cmake-build-debug/hex"
TEST=$1

if [ -z "$TEST" ]; then
    echo "Usage: $0 <test>"
    exit 1
fi


output=$($SOLUTION_PATH < $TESTS_PATH/$TEST.in)
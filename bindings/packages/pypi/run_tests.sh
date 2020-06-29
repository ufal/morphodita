#!/bin/sh

for test in test/*.py; do
  PYTHONPATH=`echo build/lib.*` python $test
done

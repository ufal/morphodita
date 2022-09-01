#!/bin/sh

PYTHONPATH=`echo ufal.morphodita/build/lib.*` python3 -m unittest discover -s ufal.morphodita/tests

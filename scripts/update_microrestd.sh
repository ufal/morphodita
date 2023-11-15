#!/bin/bash

# This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
#
# Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

rm -rf ../src/rest_server/microrestd/*
git -C ../src/rest_server/microrestd clone --depth=1 --branch=stable https://github.com/ufal/microrestd
cp -a ../src/rest_server/microrestd/microrestd/src/* ../src/rest_server/microrestd/
cp -a ../src/rest_server/microrestd/microrestd/{AUTHORS,CHANGES.md,LICENSE,README} ../src/rest_server/microrestd/
rm -rf ../src/rest_server/microrestd/microrestd/

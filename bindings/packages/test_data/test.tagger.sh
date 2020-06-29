#!/bin/sh

../../../src/train_tagger generic3 test.dict 0 test.tagger.ft 5 1 >test.tagger <<EOF
Kočka	kočka	NS1
vidí	vidět	VS3
.	.	PUNC

Kočky	kočka	NP1
vidí	vidět	VP3
.	.	PUNC

Vidím	vidět	VS1
kočku	kočka	NS4
.	.	PUNC

Vidím	vidět	VS1
kočky	kočka	NP4
.	.	PUNC
EOF

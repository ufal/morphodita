#!/bin/sh

../../../src/encode_dictionary generic 4 UNK NUM PUNC SYM >test.dict <<EOF
kočka	NS1	kočka
kočka	NS2	kočky
kočka	NS3	kočce
kočka	NS4	kočku
kočka	NS5	kočko
kočka	NS6	kočce
kočka	NS7	kočkou
kočka	NP1	kočky
kočka	NP2	koček
kočka	NP3	kočkám
kočka	NP4	kočky
kočka	NP5	kočky
kočka	NP6	kočkách
kočka	NP7	kočkami
vidět	VS1	vidím
vidět	VS2	vidíš
vidět	VS3	vidí
vidět	VP1	vidíme
vidět	VP2	vidíte
vidět	VP3	vidí
EOF

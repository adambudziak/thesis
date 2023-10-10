#!/bin/bash

N="${1}"
D="${2}"
steps="${3}"
directed="${4}"
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

cargo run --bin gen_graph -- -n $N -d $D --key "01230123012301230123012301230123" --iv "01230123012310230123012301230123" --export-dot "${N}_${D}.dot"

python "${SCRIPTPATH}/pow.py" "${N}_${D}.dot" ${N} ${D} ${steps} ${directed} > ${N}_${D}.dat



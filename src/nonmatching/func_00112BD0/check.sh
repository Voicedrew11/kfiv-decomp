#!/usr/bin/env bash
# Instruction count vs retail for one permuter output: src/nonmatching/func_00112BD0/check.sh func_00112BD0/output-490-1
cd "$(dirname "$0")/../../.." && mkdir -p build/permuter && cp "src/nonmatching/func_00112BD0/$1/source.c" build/permuter/candidate.c &&
    .venv/bin/python tools/match.py diff build/permuter/candidate.c | awk 'NR == 1'

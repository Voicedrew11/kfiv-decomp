#!/usr/bin/env bash
# Instruction count vs retail for one permuter output: src/nonmatching/Map_UpdateDrawState/check.sh Map_UpdateDrawState/output-490-1
cd "$(dirname "$0")/../../.." && mkdir -p build/permuter && cp "src/nonmatching/Map_UpdateDrawState/$1/source.c" build/permuter/candidate.c &&
    .venv/bin/python tools/match.py diff build/permuter/candidate.c | awk 'NR == 1'

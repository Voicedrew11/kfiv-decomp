#!/usr/bin/env bash
# Run decomp-permuter on func_00112BD0; results land in func_00112BD0/output-<score>-<n>/
cd "$(dirname "$0")/../../.." && exec .venv/bin/python tools/cc/decomp-permuter/permuter.py \
    src/nonmatching/func_00112BD0/func_00112BD0 -j "${JOBS:-14}" --best-only --stop-on-zero "$@"

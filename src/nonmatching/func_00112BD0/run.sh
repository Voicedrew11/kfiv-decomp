#!/usr/bin/env bash
# Run decomp-permuter on Map_UpdateDrawState; results land in Map_UpdateDrawState/output-<score>-<n>/
cd "$(dirname "$0")/../../.." && exec .venv/bin/python tools/cc/decomp-permuter/permuter.py \
    src/nonmatching/Map_UpdateDrawState/Map_UpdateDrawState -j "${JOBS:-14}" --best-only --stop-on-zero "$@"

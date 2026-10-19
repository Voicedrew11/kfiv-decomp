#!/usr/bin/env bash
# Random search on the cut-down fog block, logging every candidate to fog_random/features.csv
# (see fog_features.py). Summary: .venv/bin/python src/nonmatching/Map_UpdateDrawState/fog_features.py summary
cd "$(dirname "$0")/../../.." && PERMUTER_FEATURE_HOOK="${PWD}/src/nonmatching/Map_UpdateDrawState/fog_features.py" exec \
    .venv/bin/python tools/cc/decomp-permuter/permuter.py src/nonmatching/Map_UpdateDrawState/fog_random \
    -j "${JOBS:-14}" --best-only --stop-on-zero "$@"

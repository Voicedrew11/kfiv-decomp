#!/usr/bin/env bash
# Move the fog_random search onto the 31 version (PR #54): edit base.c, and archive the logs, findings and
# outputs that were measured against the old base into fog_random/archive-<time>/. Stop run_fog.sh first.
set -eu
cd "$(dirname "$0")/../../.."
DIR=src/nonmatching/Map_UpdateDrawState/fog_random
if pgrep -f "permuter.py ${DIR}" > /dev/null; then
    echo "run_fog.sh is still running; stop it (Ctrl+C) first" >&2
    exit 1
fi
.venv/bin/python - <<'EOF'
p = "src/nonmatching/Map_UpdateDrawState/fog_random/base.c"
s = open(p).read()
old = "blend_sum = D_0038A1B0.unk160.unk08 * fade_in + D_0038A1B0.unk140.unk08 * fade_out;"
new = "blend_sum = (blue_to = D_0038A1B0.unk160.unk08 * fade_in + D_0038A1B0.unk140.unk08 * fade_out);"
if new in s:
    print("base.c already has the 31 version")
else:
    assert s.count(old) == 1, "unexpected base.c"
    open(p, "w").write(s.replace(old, new))
    print("base.c: now the 31 version")
EOF
ARCHIVE="${DIR}/archive-$(date +%Y%m%d-%H%M)"
mkdir -p "${ARCHIVE}"
for f in features.csv baseline.json interesting "${DIR}"/output-*; do
    f="${DIR}/$(basename "${f}")"
    [ -e "${f}" ] && mv "${f}" "${ARCHIVE}/"
done
echo "old logs, findings and outputs moved to ${ARCHIVE}"
echo "now run: src/nonmatching/Map_UpdateDrawState/run_fog.sh"

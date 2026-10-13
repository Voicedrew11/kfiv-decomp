#!/usr/bin/env bash
# decomp-permuter compile step: ./compile.sh input.c -o output.o (game flags, filling ee-as 2.9)
# With PERMUTER_FEATURE_HOOK set, also leaves gcc's second scheduling dump at output.sched2
# for the hook to read (and delete).
set -eu
ROOT="$(git -C "$(dirname "$0")" rev-parse --show-toplevel)"
CC_DIR="${ROOT}/tools/cc/ee-gcc2.9-991111"
IN="$(realpath "$1")"; OUT="$(realpath -m "$3")"
S="${OUT%.o}.s"
DUMP_FLAGS=""
[ -n "${PERMUTER_FEATURE_HOOK:-}" ] && DUMP_FLAGS="-dR"
DUMP="$(dirname "${OUT}")/$(basename "${IN}").sched2"
trap 'rm -f "${S}"' EXIT
( cd "$(dirname "${OUT}")" && "${ROOT}/tools/period_env.sh" "${CC_DIR}/ee-gcc" \
    -B "${CC_DIR}/gcc-lib/ee/2.9-ee-991111-01/" -S -O2 -mips3 -EL -nostdinc -G 8 ${DUMP_FLAGS} \
    -o "${S}" "${IN}" ) || { rm -f "${DUMP}"; exit 1; }
"${ROOT}/tools/period_env.sh" "${ROOT}/build/ee-as-2.9-fill" -EL -mcpu=5900 -mabi=eabi -G 8 -o "${OUT}" "${S}" ||
    { rm -f "${DUMP}"; exit 1; }
[ -n "${DUMP_FLAGS}" ] && mv -f "${DUMP}" "${OUT%.o}.sched2"
exit 0

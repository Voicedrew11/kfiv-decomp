#!/usr/bin/env bash
# decomp-permuter compile step for the loop-barrier experiment: game flags, filling ee-as 2.9, and the
# do { } while (0)'s stray loop alignment demoted (retail has none; the cut-down function has no real loops).
set -eu
ROOT="$(git -C "$(dirname "$0")" rev-parse --show-toplevel)"
CC_DIR="${ROOT}/tools/cc/ee-gcc2.9-991111"
IN="$1"; OUT="$3"
S="${OUT%.o}.s"
trap 'rm -f "${S}"' EXIT
"${ROOT}/tools/period_env.sh" "${CC_DIR}/ee-gcc" -B "${CC_DIR}/gcc-lib/ee/2.9-ee-991111-01/" \
    -S -O2 -mips3 -EL -nostdinc -G 8 -o "${S}" "${IN}"
sed -i '6,$ s/^\t\.p2align 3$/\t.p2align 2/' "${S}"
"${ROOT}/tools/period_env.sh" "${ROOT}/build/ee-as-2.9-fill" -EL -mcpu=5900 -mabi=eabi -G 8 -o "${OUT}" "${S}"

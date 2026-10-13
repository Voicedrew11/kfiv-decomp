#!/usr/bin/env bash
# decomp-permuter compile step: ./compile.sh input.c -o output.o (game flags, filling ee-as 2.9)
set -eu
ROOT="$(git -C "$(dirname "$0")" rev-parse --show-toplevel)"
CC_DIR="${ROOT}/tools/cc/ee-gcc2.9-991111"
IN="$1"; OUT="$3"
S="${OUT%.o}.s"
trap 'rm -f "${S}"' EXIT
"${ROOT}/tools/period_env.sh" "${CC_DIR}/ee-gcc" -B "${CC_DIR}/gcc-lib/ee/2.9-ee-991111-01/" \
    -S -O2 -mips3 -EL -nostdinc -G 8 -o "${S}" "${IN}"
"${ROOT}/tools/period_env.sh" "${ROOT}/build/ee-as-2.9-fill" -EL -mcpu=5900 -mabi=eabi -G 8 -o "${OUT}" "${S}"

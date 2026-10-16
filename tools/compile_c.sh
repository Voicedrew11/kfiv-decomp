#!/usr/bin/env bash
# Compile one C source to an object with the period toolchain.
#
#   tools/compile_c.sh src/<path>.c build/src/<path>.c.o
#   tools/compile_c.sh src/<path>.s build/src/<path>.s.o   (hand-written asm:
#                                                          assembler step only)
#
# ee-gcc 2.9-ee-991111-01; the flags and the assembler follow where the code
# came from (the same split ICO's build proves):
#   src/sce/libc|libm|libgcc   compiler-install archives: -O2 -G 0 -fno-builtin, ee-as 2.9
#   src/sce/*                  Sony SDK archives:         -O2 -G 0, SCE ee-as 2.10
#   everything else (the game)                            -O2 -G 8, ee-as 2.9 (filling)
# The game is -O2 like the SDK: loops with calls (func_00102558, func_00123158,
# func_0019CCB0) match only at -O2. -g changes no game bytes, under either
# assembler below (its line labels only keep ee-as from filling a delay slot
# with an inline-asm instruction), so it is left off; GAME_CFLAGS overrides
# the default.
#
# The bundled ee-as 2.9 never fills a branch or jump delay slot: append_insn
# only fills when the previous instruction has no pinfo flags, which never
# happens. The game was assembled by something that does fill: retail has 40
# `mtc1; b|jal; cvt.s.w` sites and no `mtc1; cvt.s.w; b; nop`, and
# func_001D23F0 has `jal; ld $4,0($29)`. The game therefore uses a copy of
# ee-as 2.9 whose last fill test always passes (one byte: the je at file
# offset 0x20e0f becomes jmp), built into build/ below. It keeps 2.9's hazard
# rules and fills slots normally, and emulates that unknown assembler.
# libc/libm/libgcc stay on the stock 2.9 (five of them change under the patch).
set -eu
[ $# -eq 2 ] || { echo "usage: $0 <src.c> <out.o>" >&2; exit 2; }
SRC="$1"; OUT="$2"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CC_DIR="${ROOT}/tools/cc/ee-gcc2.9-991111"
AS_OLD="${CC_DIR}/bin/as"
AS_SDK="${ROOT}/tools/cc/ee-gcc2.96/bin/as"
AS_GAME="${ROOT}/build/ee-as-2.9-fill"
AS_OLD_SHA=296af123052ee39d175e5b3254102aafca105d4f6e975b351a13867f59b04019
AS_GAME_SHA=0e570f69f07956aa49f4ce4aa42bc296a07890fba0f4669e577834ab77350cd2

# Build ${AS_GAME} from ${AS_OLD}; parallel callers each write their own temp
# file and rename it into place.
game_as() {
    [ -f "${AS_GAME}" ] && [ ! "${AS_OLD}" -nt "${AS_GAME}" ] && return 0
    mkdir -p "${ROOT}/build"
    local tmp="${AS_GAME}.$$" sum
    cp "${AS_OLD}" "${tmp}"
    sum="$(sha256sum "${tmp}" | cut -d' ' -f1)"
    [ "${sum}" = "${AS_OLD_SHA}" ] ||
        { rm -f "${tmp}"; echo "compile_c.sh: ${AS_OLD} is not the expected ee-as 2.9 (sha256 ${sum})" >&2; exit 1; }
    printf '\353' | dd of="${tmp}" bs=1 seek=$((0x20e0f)) conv=notrunc status=none
    sum="$(sha256sum "${tmp}" | cut -d' ' -f1)"
    [ "${sum}" = "${AS_GAME_SHA}" ] ||
        { rm -f "${tmp}"; echo "compile_c.sh: patched ee-as has sha256 ${sum}, expected ${AS_GAME_SHA}" >&2; exit 1; }
    chmod +x "${tmp}"
    mv -f "${tmp}" "${AS_GAME}"
}

OPT="-O2"
case "${SRC}" in
    src/sce/libc/*|src/sce/libm/*|src/sce/libgcc/*)
        FLAGS="-G 0 -fno-builtin"; GNUM=0; AS="${AS_OLD}" ;;
    src/sce/*)
        FLAGS="-G 0"; GNUM=0; AS="${AS_SDK}" ;;
    *)
        FLAGS="${GAME_CFLAGS:--G 8}"; GNUM=8; game_as; AS="${AS_GAME}" ;;
esac

INCS="-I${ROOT}/include"
for d in "${ROOT}"/src/sce/*/; do [ -d "$d" ] && INCS="${INCS} -I${d%/}"; done
# game headers are included by their path under src/game ("entity/entity.h")
case "${SRC}" in src/sce/*) ;; *) INCS="${INCS} -I${ROOT}/src/game" ;; esac

mkdir -p "$(dirname "${OUT}")"
case "${SRC}" in
    *.s) exec "${ROOT}/tools/period_env.sh" "${AS}" -EL -mcpu=5900 -mabi=eabi -G "${GNUM}" -o "${OUT}" "${SRC}" ;;
esac
S="${OUT%.o}.s"
# ee-gcc records the path it is given in __FILE__ strings, so compile from the
# source's directory with its bare name.
SRC_DIR="$(cd "$(dirname "${SRC}")" && pwd)"
( cd "${SRC_DIR}" && "${ROOT}/tools/period_env.sh" "${CC_DIR}/ee-gcc" \
    -B "${CC_DIR}/gcc-lib/ee/2.9-ee-991111-01/" -S ${OPT} -mips3 -EL -nostdinc \
    ${FLAGS} ${INCS} -o "${ROOT}/${S}" "$(basename "${SRC}")" )
"${ROOT}/tools/period_env.sh" "${AS}" -EL -mcpu=5900 -mabi=eabi -G "${GNUM}" -o "${OUT}" "${S}"

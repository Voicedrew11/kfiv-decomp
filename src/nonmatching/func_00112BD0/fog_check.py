#!/usr/bin/env python3
"""Wrong instructions in the fog block of cut-down outputs: fog_check.py fog_random/output-*"""
import os, struct, subprocess, sys, tempfile
from elftools.elf.elffile import ELFFile

HERE = os.path.dirname(os.path.abspath(__file__))
BLOCK_START, BLOCK_WORDS = 0x148, 66  # retail 0x1131DC-0x1132E0 in the cut-down object


def block_words(obj):
    with open(obj, 'rb') as f:
        data = ELFFile(f).get_section_by_name('.text').data()
    return [struct.unpack('<I', data[k:k + 4])[0] for k in range(BLOCK_START, BLOCK_START + BLOCK_WORDS * 4, 4)]


target = block_words(os.path.join(HERE, 'fog', 'target.o'))
for out_dir in sys.argv[1:]:
    with tempfile.TemporaryDirectory() as tmp:
        obj = os.path.join(tmp, 'c.o')
        subprocess.run([os.environ.get("FOG_COMPILE", os.path.join(HERE, "fog", "compile.sh")), os.path.join(out_dir, 'source.c'), '-o', obj], check=True)
        words = block_words(obj)
    wrong = sum(a != b for a, b in zip(words, target))
    print(f'{wrong:3d} of {BLOCK_WORDS} wrong  {out_dir}')

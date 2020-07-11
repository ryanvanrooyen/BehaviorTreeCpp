#!/usr/bin/env python3

import sys
from shutil import copyfileobj
from os.path import abspath, dirname, join
from tempfile import TemporaryFile


def preprocess(destination_file, source_file):
    if not destination_file:
        raise Exception('No destination file specified.')
    if not source_file:
        raise Exception('No source file specified.')

    source_file = abspath(source_file)
    lib_headers, local_headers = set(), set()
    with TemporaryFile(mode='w+') as temp:
        first_header_pos = preprocess_file(temp, source_file, lib_headers, local_headers)
        temp.seek(0)
        with open(destination_file, mode='w') as dest:
            dest.write(temp.read(first_header_pos))
            for header in sorted(lib_headers):
                dest.write(f'#include <{header}>\n')
            copyfileobj(temp, dest)


def preprocess_file(out, source_file, lib_headers, local_headers):
    if source_file in local_headers:
        return
    local_headers.add(source_file)
    first_header_pos, pos = None, 0
    with open(source_file) as f:
        for line in f:
            stripped_line = line.strip()
            if not stripped_line.startswith('#include'):
                out.write(line)
                pos += len(line)
                continue
            if first_header_pos is None:
                first_header_pos = pos
            included_file = substr(line, '<', '>')
            if included_file:
                lib_headers.add(included_file.strip())
                continue
            included_file = substr(line, '"', '"')
            if included_file:
                full_path = abspath(join(dirname(source_file), included_file.strip()))
                preprocess_file(out, full_path, lib_headers, local_headers)
    return first_header_pos


def substr(s, start_str, end_str):
    try:
        start = s.index(start_str) + len(start_str)
        end = s.index(end_str, start)
        return s[start:end]
    except:
        return None


if __name__ == '__main__':
    try:
        dest = sys.argv[1] if len(sys.argv) > 1 else None
        src = sys.argv[2] if len(sys.argv) > 2 else None
        preprocess(dest, src)
    except Exception as e:
        print(e)
        sys.exit(1)

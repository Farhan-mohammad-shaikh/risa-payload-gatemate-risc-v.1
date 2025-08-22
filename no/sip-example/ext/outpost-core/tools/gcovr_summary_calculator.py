#!/usr/bin/python3
import fileinput
import re

def split_coverage(line):
    match = re.search(r'(.*): (.*) \(([0-9]*) out of ([0-9]*)\)', line)
    if not match:
        return None
    return (match.group(1), match.group(3), match.group(4))

def division_guard(n, d):
    return n / d if d else 0

for line in fileinput.input():
    maybe_coverage_line = split_coverage(line)
    if maybe_coverage_line:
        (text, n, of) = maybe_coverage_line
        print (f'{text}: {division_guard(int(n), int(of)) * 100.}%')
    else:
        print (line, end='')

import os
from itertools import dropwhile, takewhile

paths = []

for root, _, file_names in os.walk(os.path.dirname(os.path.realpath(__file__))):
    if "config" in root:
        continue

    for file_name in file_names:
        _, extension = os.path.splitext(file_name)

        if extension in (".h", ".c"):
            paths.append(os.path.join(root, file_name))


for path in paths:
    with open(path) as file:
        lines = file.readlines()

    include_lines = [l for l in lines if l.lstrip().startswith("#include")]

    lines_before_first_include = list(takewhile(lambda l: not l.lstrip().startswith("#include"), lines))

    lines_from_first_include = list(dropwhile(lambda l: not l.lstrip().startswith("#include"), lines))

    lines_from_first_include = [l for l in lines_from_first_include if not l.lstrip().startswith("#include")]

    for index, include_line in enumerate(include_lines):
        include_lines[index] = str(include_line.lstrip()).replace("<", '"').replace(">", '"')

    for index, include_line in enumerate(include_lines):
        include_lines[index] = include_line.split('"')[0] + '"' + include_line.split('"')[1] + '"\n'

    include_lines.sort(key=lambda line: line.upper())

    standard_libraries = (
        '"assert.h"',
        '"complex.h"',
        '"ctype.h"',
        '"errno.h"',
        '"fenv.h"',
        '"float.h"',
        '"inttypes.h"',
        '"iso646.h"',
        '"limits.h"',
        '"locale.h"',
        '"math.h"',
        '"setjmp.h"',
        '"signal.h"',
        '"stdalign.h"',
        '"stdarg.h"',
        '"stdatomic.h"',
        '"stdbool.h"',
        '"stddef.h"',
        '"stdint.h"',
        '"stdio.h"',
        '"stdlib.h"',
        '"stdnoreturn.h"',
        '"string.h"',
        '"strings.h"',
        '"tgmath.h"',
        '"threads.h"',
        '"time.h"',
        '"uchar.h"',
        '"wchar.h"',
        '"wctype.h",',
    )  # https://en.wikipedia.org/wiki/C_standard_library

    for index, include_line in enumerate(include_lines):
        if any(s in include_line for s in standard_libraries):
            include_lines[index] = include_line.replace('"', ">").replace(" >", " <")

    with open(path, "w") as file:
        for line in lines_before_first_include:
            file.write(line)
        for line in include_lines:
            file.write(line)
        for line in lines_from_first_include:
            file.write(line)

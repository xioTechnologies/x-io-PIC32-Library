import os


def process_file(file_path):

    # Read file
    with open(file_path) as source_file:
        all_lines = source_file.readlines()

    # Extract lines
    hash_include = "#include"

    include_lines = []
    lines_before = []
    lines_after = []

    waiting_for_includes = True

    for line in all_lines:
        if line.lstrip().startswith(hash_include):
            include_lines.append(str(line.lstrip()).replace("<", "\"").replace(">", "\""))
            waiting_for_includes = False
            continue
        if waiting_for_includes:
            lines_before.append(line)
        else:
            lines_after.append(line)

    # Alphabetise includes
    include_lines.sort(key=lambda line: line.upper())

    # Use angle brackets for standard libraries (https://en.wikipedia.org/wiki/C_standard_library)
    standard_libraries = ["\"assert.h\"", "\"complex.h\"", "\"ctype.h\"", "\"errno.h\"", "\"fenv.h\"", "\"float.h\"", "\"inttypes.h\"",
                          "\"iso646.h\"", "\"limits.h\"", "\"locale.h\"", "\"math.h\"", "\"setjmp.h\"", "\"signal.h\"", "\"stdalign.h\"",
                          "\"stdarg.h\"", "\"stdatomic.h\"", "\"stdbool.h\"", "\"stddef.h\"", "\"stdint.h\"", "\"stdio.h\"", "\"stdlib.h\"",
                          "\"stdnoreturn.h\"", "\"string.h\"", "\"tgmath.h\"", "\"threads.h\"", "\"time.h\"", "\"uchar.h\"", "\"wchar.h\"",
                          "\"wctype.h\"", "\"xc.h\""]

    for index, _ in enumerate(include_lines):
        for standard_library in standard_libraries:
            if standard_library in include_lines[index]:
                include_lines[index] = include_lines[index].replace("\"", "<").replace("h<", "h>")

    # Overwrite original file
    with open(file_path, "w") as source_file:
        for line in lines_before:
            source_file.write(line)
        for line in include_lines:
            source_file.write(line)
        for line in lines_after:
            source_file.write(line)


# Recursively process each file
for root, _, files in os.walk(os.path.dirname(os.path.realpath(__file__))):
    if "system_config" in root:
        continue
    for source_file in files:
        file_name, file_extension = os.path.splitext(source_file)
        if file_extension == ".h" or file_extension == ".c":
            process_file(os.path.join(root, source_file))

import os

for root, _, files in os.walk(os.path.dirname(os.path.realpath(__file__))):
    if "config" in root:
        continue

    for source_file in files:
        file_name, file_extension = os.path.splitext(source_file)

        if file_extension != ".h" and file_extension != ".c":
            continue

        # Read file
        file_path = os.path.join(root, source_file)
        with open(file_path) as source_file:
            all_lines = source_file.readlines()

        # Extract lines
        include_lines = []
        lines_before = []
        lines_after = []

        waiting_for_includes = True

        for line in all_lines:
            line = line.rstrip() + "\n"  # remove trailing whitespace
            if line.lstrip().startswith("#include"):
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
                              "\"stdnoreturn.h\"", "\"string.h\"", "\"strings.h\"", "\"tgmath.h\"", "\"threads.h\"", "\"time.h\"", "\"uchar.h\"",
                              "\"wchar.h\"", "\"wctype.h\""]

        for index, _ in enumerate(include_lines):
            for standard_library in standard_libraries:
                if standard_library in include_lines[index]:
                    include_lines[index] = include_lines[index].replace("\"", ">").replace(" >", " <")

        # Overwrite original file
        with open(file_path, "w") as source_file:
            for line in lines_before:
                source_file.write(line)
            for line in include_lines:
                source_file.write(line)
            for line in lines_after:
                source_file.write(line)

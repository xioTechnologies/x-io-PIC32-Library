def find_and_replace(source_files, destination_files, old_text, new_text):
    for files_index, _ in enumerate(source_files):
        with open(source_files[files_index]) as source_file:
            contents = source_file.read()

        for text_index, _ in enumerate(old_text):
            contents = contents.replace(
                old_text[text_index], new_text[text_index])

        with open(destination_files[files_index], "w") as source_file:
            source_file.write(contents)

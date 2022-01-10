def replace_id(strings, id):
    new_strings = []

    for string in strings:
        new_strings.append(string.replace("?", str(id)))

    return new_strings


def copy(files, keywords, source_id, destination_ids):
    source_files = replace_id(files, source_id)
    source_keywords = replace_id(keywords, source_id)

    for id in destination_ids:
        destination_files = replace_id(files, id)
        destination_keywords = replace_id(keywords, id)

        for files_index, _ in enumerate(source_files):
            with open(source_files[files_index]) as file:
                contents = file.read()

            for keyword_index, _ in enumerate(source_keywords):
                contents = contents.replace(source_keywords[keyword_index], destination_keywords[keyword_index])

            with open(destination_files[files_index], "w") as file:
                file.write(contents)


files = ["I2C/I2C?.c", "I2C/I2C?.h"]
keywords = ["I2C?"]

copy(files, keywords, 1, [2, 3, 4, 5])

files = ["Spi/Spi?Dma.c", "Spi/Spi?Dma.h"]
keywords = ["Spi?", "SPI?"]

copy(files, keywords, 1, [2, 3, 4, 5, 6])

files = ["Uart/Uart?.c", "Uart/Uart?.h",
         "Uart/Uart?Dma.c", "Uart/Uart?Dma.h",
         "Uart/Uart?DmaRX.c", "Uart/Uart?DmaRX.h",
         "Uart/Uart?DmaTX.c", "Uart/Uart?DmaTX.h"]
keywords = ["Uart?", "U?", "UART?", "UART_?"]

copy(files, keywords, 1, [2, 3, 4, 5, 6])

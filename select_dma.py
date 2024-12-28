def replace_keywords(file_path, keywords, old_ids, new_ids):
    with open(file_path) as file:
        contents = file.read()

    for index, _ in enumerate(old_ids):
        old_keywords = [k.replace("?", str(old_ids[index])) for k in keywords]
        new_keywords = [k.replace("?", str(new_ids[index])) for k in keywords]

        for keyword_index, _ in enumerate(old_keywords):
            contents = contents.replace(old_keywords[keyword_index], new_keywords[keyword_index])

    with open(file_path, "w") as file:
        file.write(contents)


keywords = ["DCH?", "Dma?", "DMA?"]

replace_keywords("Spi/Spi1Dma.c", keywords, [0, 1], [2, 3])

replace_keywords("Uart/Uart1Dma.c", keywords, [0, 1, 2], [3, 4, 5])

replace_keywords("Uart/Uart1DmaRX.c", keywords, [0, 1], [2, 3])

replace_keywords("Uart/Uart1DmaTX.c", keywords, [0], [1])

import re


def dma_Select(path, new_channels):
    with open(path) as file:
        code = file.read()

    keywords = ("DCH?", "Dma?", "DMA?")

    pattern = "|".join(k.replace("?", r"([0-7])") for k in keywords)

    matches = re.findall(pattern, code)

    old_channels = sorted({c for m in matches for c in m if c})

    for old_channel, new_channel in zip(old_channels, new_channels):
        old_keywords = [k.replace("?", old_channel) for k in keywords]
        new_keywords = [k.replace("?", f"${new_channel}$") for k in keywords]

        for old_keyword, new_keyword in zip(old_keywords, new_keywords):
            code = code.replace(old_keyword, new_keyword)

    code = code.replace("$", "")

    with open(path, "w") as file:
        file.write(code)


dma_Select("Spi/Spi1Dma.c", (0, 1))
dma_Select("Spi/Spi2Dma.c", (0, 1))
dma_Select("Spi/Spi3Dma.c", (0, 1))
dma_Select("Spi/Spi4Dma.c", (0, 1))
dma_Select("Spi/Spi5Dma.c", (0, 1))
dma_Select("Spi/Spi6Dma.c", (0, 1))

dma_Select("Spi/Spi1DmaTX.c", (0,))
dma_Select("Spi/Spi2DmaTX.c", (0,))
dma_Select("Spi/Spi3DmaTX.c", (0,))
dma_Select("Spi/Spi4DmaTX.c", (0,))
dma_Select("Spi/Spi5DmaTX.c", (0,))
dma_Select("Spi/Spi6DmaTX.c", (0,))

dma_Select("Uart/Uart1Dma.c", (0, 1, 2))
dma_Select("Uart/Uart2Dma.c", (0, 1, 2))
dma_Select("Uart/Uart3Dma.c", (0, 1, 2))
dma_Select("Uart/Uart4Dma.c", (0, 1, 2))
dma_Select("Uart/Uart5Dma.c", (0, 1, 2))
dma_Select("Uart/Uart6Dma.c", (0, 1, 2))

dma_Select("Uart/Uart1DmaTX.c", (0,))
dma_Select("Uart/Uart2DmaTX.c", (0,))
dma_Select("Uart/Uart3DmaTX.c", (0,))
dma_Select("Uart/Uart4DmaTX.c", (0,))
dma_Select("Uart/Uart5DmaTX.c", (0,))
dma_Select("Uart/Uart6DmaTX.c", (0,))

dma_Select("Uart/Uart1DmaRX.c", (0, 1))
dma_Select("Uart/Uart2DmaRX.c", (0, 1))
dma_Select("Uart/Uart3DmaRX.c", (0, 1))
dma_Select("Uart/Uart4DmaRX.c", (0, 1))
dma_Select("Uart/Uart5DmaRX.c", (0, 1))
dma_Select("Uart/Uart6DmaRX.c", (0, 1))

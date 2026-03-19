import re


def dma_select(path: str, new_channels: tuple[int, ...]):
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


dma_select("Spi/Spi1Dma.c", (0, 1))
dma_select("Spi/Spi2Dma.c", (0, 1))
dma_select("Spi/Spi3Dma.c", (0, 1))
dma_select("Spi/Spi4Dma.c", (0, 1))
dma_select("Spi/Spi5Dma.c", (0, 1))
dma_select("Spi/Spi6Dma.c", (0, 1))

dma_select("Spi/Spi1DmaTx.c", (0,))
dma_select("Spi/Spi2DmaTx.c", (0,))
dma_select("Spi/Spi3DmaTx.c", (0,))
dma_select("Spi/Spi4DmaTx.c", (0,))
dma_select("Spi/Spi5DmaTx.c", (0,))
dma_select("Spi/Spi6DmaTx.c", (0,))

dma_select("Uart/Uart1Dma.c", (0, 1, 2))
dma_select("Uart/Uart2Dma.c", (0, 1, 2))
dma_select("Uart/Uart3Dma.c", (0, 1, 2))
dma_select("Uart/Uart4Dma.c", (0, 1, 2))
dma_select("Uart/Uart5Dma.c", (0, 1, 2))
dma_select("Uart/Uart6Dma.c", (0, 1, 2))

dma_select("Uart/Uart1DmaTx.c", (0,))
dma_select("Uart/Uart2DmaTx.c", (0,))
dma_select("Uart/Uart3DmaTx.c", (0,))
dma_select("Uart/Uart4DmaTx.c", (0,))
dma_select("Uart/Uart5DmaTx.c", (0,))
dma_select("Uart/Uart6DmaTx.c", (0,))

dma_select("Uart/Uart1DmaRx.c", (0, 1))
dma_select("Uart/Uart2DmaRx.c", (0, 1))
dma_select("Uart/Uart3DmaRx.c", (0, 1))
dma_select("Uart/Uart4DmaRx.c", (0, 1))
dma_select("Uart/Uart5DmaRx.c", (0, 1))
dma_select("Uart/Uart6DmaRx.c", (0, 1))

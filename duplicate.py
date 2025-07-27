def duplicate(files, keywords, source_id, destination_ids):
    source_files = [f.replace("?", str(source_id)) for f in files]
    source_keywords = [k.replace("?", str(source_id)) for k in keywords]

    for id in destination_ids:
        destination_files = [f.replace("?", str(id)) for f in files]
        destination_keywords = [k.replace("?", str(id)) for k in keywords]

        for index, _ in enumerate(source_files):
            with open(source_files[index]) as file:
                code = file.read()

            for keyword_index, _ in enumerate(source_keywords):
                code = code.replace(source_keywords[keyword_index], destination_keywords[keyword_index])

            with open(destination_files[index], "w") as file:
                file.write(code)


duplicate(
    ("I2C/I2C?.c", "I2C/I2C?.h"),
    ("I2C?", "i2c?"),
    1,
    (2, 3, 4, 5),
)

duplicate(
    ("I2C/I2CBB?.c", "I2C/I2CBB?.h"),
    ("I2CBB?", "i2cBB?"),
    1,
    (2,),
)

duplicate(
    ("InputCapture/InputCapture?.c", "InputCapture/InputCapture?.h"),
    ("InputCapture?", "IC?", "INPUT_CAPTURE_?"),
    1,
    (2, 3, 4, 5, 6, 7, 8, 9),
)

duplicate(
    ("Pwm/Pwm?.c", "Pwm/Pwm?.h"),
    ("Pwm?", "PWM_?", "OC?"),
    1,
    (2, 3, 4, 5, 6, 7, 8, 9),
)

duplicate(
    ("Spi/Spi?.c", "Spi/Spi?.h", "Spi/Spi?Dma.c", "Spi/Spi?Dma.h", "Spi/Spi?DmaTx.c", "Spi/Spi?DmaTx.h"),
    ("Spi?", "SPI?", "spi?"),
    1,
    (2, 3, 4, 5, 6),
)

duplicate(
    ("Spi/SpiBus?.c", "Spi/SpiBus?.h"),
    ("SpiBus?", "SPI_BUS_?", "spiBus?"),
    1,
    (2, 3, 4, 5, 6),
)

duplicate(
    ("NeoPixels/NeoPixels?.c", "NeoPixels/NeoPixels?.h"),
    ("NeoPixels?", "NEOPIXELS_?", "neoPixels?"),
    1,
    (2, 3, 4, 5, 6),
)

duplicate(
    ("Uart/Uart?.c", "Uart/Uart?.h", "Uart/Uart?Dma.c", "Uart/Uart?Dma.h", "Uart/Uart?DmaRx.c", "Uart/Uart?DmaRx.h", "Uart/Uart?DmaTx.c", "Uart/Uart?DmaTx.h"),
    ("Uart?", "U?", "UART?", "UART_?"),
    1,
    (2, 3, 4, 5, 6),
)

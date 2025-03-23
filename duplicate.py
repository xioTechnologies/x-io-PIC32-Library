def duplicate(files, keywords, source_id, destination_ids):
    source_files = [f.replace("?", str(source_id)) for f in files]
    source_keywords = [k.replace("?", str(source_id)) for k in keywords]

    for id in destination_ids:
        destination_files = [f.replace("?", str(id)) for f in files]
        destination_keywords = [k.replace("?", str(id)) for k in keywords]

        for files_index, _ in enumerate(source_files):
            with open(source_files[files_index]) as file:
                contents = file.read()

            for keyword_index, _ in enumerate(source_keywords):
                contents = contents.replace(source_keywords[keyword_index], destination_keywords[keyword_index])

            with open(destination_files[files_index], "w") as file:
                file.write(contents)


duplicate(
    ("I2C/I2C?.c", "I2C/I2C?.h"),
    ("I2C?",),
    1,
    (2, 3, 4, 5),
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
    ("Spi/Spi?Dma.c", "Spi/Spi?Dma.h"),
    ("Spi?", "SPI?"),
    1,
    (2, 3, 4, 5, 6),
)

duplicate(
    ("Uart/Uart?.c", "Uart/Uart?.h", "Uart/Uart?Dma.c", "Uart/Uart?Dma.h", "Uart/Uart?DmaRX.c", "Uart/Uart?DmaRX.h", "Uart/Uart?DmaTX.c", "Uart/Uart?DmaTX.h"),
    ("Uart?", "U?", "UART?", "UART_?"),
    1,
    (2, 3, 4, 5, 6),
)

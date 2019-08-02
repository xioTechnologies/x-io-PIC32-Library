from find_and_replace import find_and_replace

spi1_files = ["Spi/Spi1Dma.c", "Spi/Spi1Dma.h"]
spi1_text = ["Spi1", "SPI1"]

spi2_files = ["Spi/Spi2Dma.c", "Spi/Spi2Dma.h"]
spi2_text = ["Spi2", "SPI2"]

spi3_files = ["Spi/Spi3Dma.c", "Spi/Spi3Dma.h"]
spi3_text = ["Spi3", "SPI3"]

spi4_files = ["Spi/Spi4Dma.c", "Spi/Spi4Dma.h"]
spi4_text = ["Spi4", "SPI4"]

spi5_files = ["Spi/Spi5Dma.c", "Spi/Spi5Dma.h"]
spi5_text = ["Spi5", "SPI5"]

spi6_files = ["Spi/Spi6Dma.c", "Spi/Spi6Dma.h"]
spi6_text = ["Spi6", "SPI6"]

source_files = spi2_files  # files to be duplicated
old_text = spi2_text  # text to be replaced in duplicated files

find_and_replace(source_files, spi1_files, old_text, spi1_text)
find_and_replace(source_files, spi2_files, old_text, spi2_text)
find_and_replace(source_files, spi3_files, old_text, spi3_text)
find_and_replace(source_files, spi4_files, old_text, spi4_text)
find_and_replace(source_files, spi5_files, old_text, spi5_text)
find_and_replace(source_files, spi6_files, old_text, spi6_text)

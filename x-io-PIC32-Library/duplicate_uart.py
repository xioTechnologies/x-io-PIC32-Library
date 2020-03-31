from find_and_replace import find_and_replace

uart1_files = ["Uart/Uart1.c", "Uart/Uart1.h", "Uart/Uart1Dma.c", "Uart/Uart1Dma.h"]
uart1_text = ["Uart1", "U1", "UART1", "UART_1", "USART_1"]

uart2_files = ["Uart/Uart2.c", "Uart/Uart2.h", "Uart/Uart2Dma.c", "Uart/Uart2Dma.h"]
uart2_text = ["Uart2", "U2", "UART2", "UART_2", "USART_2"]

uart3_files = ["Uart/Uart3.c", "Uart/Uart3.h", "Uart/Uart3Dma.c", "Uart/Uart3Dma.h"]
uart3_text = ["Uart3", "U3", "UART3", "UART_3", "USART_3"]

uart4_files = ["Uart/Uart4.c", "Uart/Uart4.h", "Uart/Uart4Dma.c", "Uart/Uart4Dma.h"]
uart4_text = ["Uart4", "U4", "UART4", "UART_4", "USART_4"]

uart5_files = ["Uart/Uart5.c", "Uart/Uart5.h", "Uart/Uart5Dma.c", "Uart/Uart5Dma.h"]
uart5_text = ["Uart5", "U5", "UART5", "UART_5", "USART_5"]

uart6_files = ["Uart/Uart6.c", "Uart/Uart6.h", "Uart/Uart6Dma.c", "Uart/Uart6Dma.h"]
uart6_text = ["Uart6", "U6", "UART6", "UART_6", "USART_6"]

source_files = uart1_files  # files to be duplicated
old_text = uart1_text  # text to be replaced in duplicated files

find_and_replace(source_files, uart1_files, old_text, uart1_text)
find_and_replace(source_files, uart2_files, old_text, uart2_text)
find_and_replace(source_files, uart3_files, old_text, uart3_text)
find_and_replace(source_files, uart4_files, old_text, uart4_text)
find_and_replace(source_files, uart5_files, old_text, uart5_text)
find_and_replace(source_files, uart6_files, old_text, uart6_text)

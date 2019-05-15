from find_and_replace import find_and_replace

i2c1_files = ["I2C/I2C1.c", "I2C/I2C1.h"]
i2c1_text = ["I2C1", "I2C1", "I2C_1"]

i2c2_files = ["I2C/I2C2.c", "I2C/I2C2.h"]
i2c2_text = ["I2C2", "I2C2", "I2C_2"]

i2c3_files = ["I2C/I2C3.c", "I2C/I2C3.h"]
i2c3_text = ["I2C3", "I2C3", "I2C_3"]

i2c4_files = ["I2C/I2C4.c", "I2C/I2C4.h"]
i2c4_text = ["I2C4", "I2C4", "I2C_4"]

i2c5_files = ["I2C/I2C5.c", "I2C/I2C5.h"]
i2c5_text = ["I2C5", "I2C5", "I2C_5"]

source_files = i2c1_files
old_text = i2c1_text

find_and_replace(source_files, i2c1_files, old_text, i2c1_text)
find_and_replace(source_files, i2c2_files, old_text, i2c2_text)
find_and_replace(source_files, i2c3_files, old_text, i2c3_text)
find_and_replace(source_files, i2c4_files, old_text, i2c4_text)
find_and_replace(source_files, i2c5_files, old_text, i2c5_text)

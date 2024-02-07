CC = gcc

VPATH := src:build
OUTPUT_DIR = build

OBJECTS = smart_fan.o
OBJECTS += ssd1306_i2c.o

.PHONY : init
init:
	mkdir -p "build"

$(OBJECTS): %.o: %.c
	$(CC) -c $< -o $(OUTPUT_DIR)/$@ -lwiringPi

main: init $(OBJECTS)
	$(CC) -o $(OUTPUT_DIR)/main $(addprefix build/, $(OBJECTS)) -lwiringPi

.PHONY : clean
clean:
	-rm -r $(OUTPUT_DIR)

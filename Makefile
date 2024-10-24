CC=gcc
CFLAGS=-Wall -Wextra -O2 -lm
PYTHON=python3
SRC_DIR=src
BUILD_DIR=build

all: $(BUILD_DIR)/battery_calculator

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/battery_calculator: $(SRC_DIR)/battery_calculator.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# Run the Python GUI application
run-gui: all
	$(PYTHON) $(SRC_DIR)/battery_gui.py

# Run the C program directly with test data (for testing)
run-test: all
	@echo "Creating test input file..."
	@echo "100" > test_input.txt  # Target power (Wh)
	@echo "3500" >> test_input.txt  # Capacity (mAh)
	@echo "3.7" >> test_input.txt   # Voltage (V)
	@echo "10" >> test_input.txt    # Max Current (A)
	@echo "47" >> test_input.txt    # Weight (g)
	@echo "65" >> test_input.txt    # Length (mm)
	@echo "18" >> test_input.txt    # Width (mm)
	@echo "18" >> test_input.txt    # Height (mm)
	./$(BUILD_DIR)/battery_calculator test_input.txt test_output.txt
	@echo "Test output:"
	@cat test_output.txt
	@rm -f test_input.txt test_output.txt

# Default run target (uses GUI)
run: run-gui

clean:
	rm -rf $(BUILD_DIR)
	rm -f cell_specs.txt output.txt test_input.txt test_output.txt

.PHONY: all clean run run-gui run-test
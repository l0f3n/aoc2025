CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -O2

# Targets 1-12
TARGETS = 1

.PHONY: all run clean $(addprefix run-,$(TARGETS))

# Default target builds target 1
all: 1

# Pattern rule: build target N from N.cpp
%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Individual target rules (e.g., make 1, make 2, etc.)
$(TARGETS): %: %.cpp

# Default run target runs target 1
run: 1
	@if [ ! -f 1.txt ]; then \
		echo "Error: 1.txt not found"; \
		exit 1; \
	fi
	./1 < 1.txt

# Pattern rule for running specific targets (e.g., make run-5)
run-%: %
	@if [ ! -f $*.txt ]; then \
		echo "Error: $*.txt not found"; \
		exit 1; \
	fi
	./$* < $*.txt

clean:
	rm -f $(TARGETS)


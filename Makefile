SHELL := /bin/bash


# ============================================
# COMMANDS

CC = gcc -O3
RM = rm -f

# ==========================================
# TARGETS

EXECUTABLES = charaldpShellProject

default: $(EXECUTABLES)
all: default


%: %.c
	$(CC) $< -o $@

clean:
	$(RM) *.o *~ $(EXECUTABLES)

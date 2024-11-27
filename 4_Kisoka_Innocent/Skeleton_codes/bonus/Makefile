CC     = mpicc
CFLAGS =



# Target to build the bonus executable
bonus: bonus.c
	$(CC) $(CFLAGS) $^ -o $@

# Build both targets
all: bonus

.PHONY: clean
clean:
	$(RM) bonus
scheduler: escalonamento.c
	cc -Wall -Wextra escalonamento.c -o scheduler

clean:
	rm -f scheduler

.PHONY: clean
scheduler: escalonamento.c
	cc escalonamento.c -o scheduler

clean:
	rm -f scheduler

.PHONY: clean
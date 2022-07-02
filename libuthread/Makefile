# Target library
lib := libuthread.a #original
objs := queue.o uthread.o context.o preempt.o

CC		:= gcc
CFLAGS	:= -Wall -Wextra -Werror -MMD
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif

ifneq ($(V),1)
Q = @
endif

all: $(lib) #original

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

## TODO
libuthread.a: $(objs)
	ar rcs libuthread.a $(objs)

%.o: %.c %.h
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

context.o: context.c private.h
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

preempt.o: preempt.c private.h
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<


clean: 
	@echo "CLEAN"
	$(Q)rm -f $(lib) $(objs) $(deps)

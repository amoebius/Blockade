# Rudimentary make-file

SOURCES = arbiter.cpp ChildProcess.cpp \
          pipe/duopipe.cpp pipe/iopipe.cpp pipe/ipipe.cpp pipe/opipe.cpp \
	      threading/HT/htcondition.c threading/HT/hthread.c threading/HT/htmutex.c
HEADERS = arbiter.hpp ChildProcess.hpp \
          pipe/duopipe.hpp pipe/iopipe.hpp pipe/ipipe.hpp pipe/opipe.hpp \
	      threading/atomic.hpp threading/hawkthreads.hpp threading/threading.hpp \
	      threading/HT/hawklib.h threading/HT/hawkthreads.h threading/HT/htinternal.h

all: $(SOURCES) $(HEADERS)
	g++ -O3 $(SOURCES) -o arbiter

%.c:
	@echo

%.cpp:
	@echo

%.h:
	@echo

%.hpp:
	@echo
CXXFLAGS = -Wall -Wextra

RM=rm -f
LDFLAGS=

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard *.cpp)
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

# Allows one to enable verbose builds with VERBOSE=1
V := @
ifeq ($(VERBOSE),1)
	V :=
endif

all: release

cgoto: CXXFLAGS += -DSAT_USE_COMPUTED_GOTO
cgoto: release

profile: CXXFLAGS += -O2 -g3
profile: cgoto

release: CXXFLAGS += -O3
release: sat

debug: CXXFLAGS += -g3 -DDEBUG
debug: sat

debug_ins: CXXFLAGS += -DDEBUG_INS
debug_ins: debug

sat: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o sat

benchmark: release
	$(V) ./util/benchmark.py -l sat $(suite)

benchmark_baseline: release
	$(V) ./util/benchmark.py --generate-baseline

pgo: merge_profraw pgouse

ifeq ($(findstring clang++,$(CXX)),clang++)
clean_pgodata: clean
	$(V) rm -f default_*.profraw default.profdata
else
clean_pgodata: clean
	$(V) rm -f *.gcda objects/*.gcda stdlib/*/*.gcda
endif

pgobuild: CXXFLAGS+=-fprofile-generate -march=native
pgobuild: LDFLAGS+=-fprofile-generate -flto
ifeq ($(findstring clang++,$(CXX)),clang++)
pgobuild: CXXFLAGS+=-mllvm -vp-counters-per-site=5
endif
pgobuild: | clean_pgodata cgoto

pgorun: | pgobuild benchmark

ifeq ($(findstring clang++,$(CXX)),clang++)
merge_profraw: pgorun
	$(V) llvm-profdata merge --output=default.profdata default_*.profraw
else
merge_profraw: pgorun
endif

pgouse: merge_profraw
	$(V) $(MAKE) clean
	$(V) $(MAKE) cgoto CXXFLAGS=-fprofile-use CXXFLAGS+=-march=native LDFLAGS+=-fprofile-use LDFLAGS+=-flto
	$(V) $(MAKE) benchmark

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

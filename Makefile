#
# Makefile to build oc
#

CC=gcc # clang can be used here as well


SRCFILES =	src/lists/vector.c\
		src/os/specific.c\
		src/str/buffer.c\
		src/map/hash.c\
		src/str/strset.c\
		src/str/strvec.c\
		src/str/stringutil.c\
		src/parse/options/flag.c\
		src/parse/options/copt.c\
		src/utilz/handy.c\
		src/path/pathutil.c\
		src/path/walk.c\
		src/parse/source/code.c\
		src/utilz/timer.c\
		src/utilz/global.c\
		src/compiler/pkg.c\
		src/os/spawn/jobs.c\
		src/compiler/dag.c\
		src/start/main.c


OBJFILES = $(subst .c,.o, $(SRCFILES))

default: link

localinstall: link
	@ [ -d "${HOME}/bin" ] && cp bin/oc "${HOME}/bin"

link: compile
	@ $(CC) $(OBJFILES) -lpthread -o bin/oc

compile: $(OBJFILES)

%.o: %.c
	@ $(CC) -Wall -I src -c $< -o $@
 
clean:
	@ rm -rf $(OBJFILES) bin/oc

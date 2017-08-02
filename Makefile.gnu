# ****************************************************************
# Makefile for SPL on Linux

# Base variables for the build process
PLATFORM = linux
MAKE_PARALLEL = $(J) 

# This file can be generated by ./gensrclist.sh
include Makefile.srcs

# Sets the target platform for SPL
# Valid values for variable platform are unixlike and windows
CC=gcc

# Additional compiler flags, add '-DPIPEDEBUG' for a debug build showing piped commands
CFLAGS=-std=gnu11 #-DPIPEDEBUG 
LDLIBS =-lfreeimage -lstdc++

.PHONY: freeimage

# ***************************************************************
# Entryies to build the resources

freeimage:  
	@echo "Build FreeImage started"
	@echo "This could take some realy long time ..."
	@echo "Building ..."
	make -j$(MAKE_PARALLEL) -s -C resources/FreeImage FREEIMAGE_LIBRARY_TYPE=STATIC
	@echo "Build FreeImage finished"

freeimage_clean:
	@echo "Cleaning FreeImage started"
	@echo "Cleaning ..."
	make -s -C resources/FreeImage clean FREEIMAGE_LIBRARY_TYPE=STATIC
	@echo "Cleaning FreeImage finished"

# ***************************************************************
# Entry to build the directory structure

$(BUILD): 
	@echo "Build Directories"
	@mkdir -p $(BUILD)

# ***************************************************************
# Entryies to build the library

build/$(PLATFORM)/obj/bst.o: c/src/bst.c c/include/bst.h c/include/cmpfn.h c/include/cslib.h \
           c/include/exception.h c/include/foreach.h c/include/generic.h \
           c/include/iterator.h c/include/itertype.h c/include/strlib.h \
           c/include/unittest.h
	@echo "Build bst.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/bst.o -Ic/include c/src/bst.c

build/$(PLATFORM)/obj/charset.o: c/src/charset.c c/include/charset.h c/include/cmpfn.h \
               c/include/cslib.h c/include/exception.h c/include/foreach.h \
               c/include/generic.h c/include/iterator.h c/include/itertype.h \
               c/include/strlib.h c/include/unittest.h
	@echo "Build charset.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/charset.o -Ic/include c/src/charset.c

build/$(PLATFORM)/obj/cmdscan.o: c/src/cmdscan.c c/include/cmdscan.h c/include/cmpfn.h \
               c/include/cslib.h c/include/exception.h c/include/generic.h \
               c/include/hashmap.h c/include/iterator.h c/include/itertype.h \
               c/include/private/tokenpatch.h c/include/simpio.h \
               c/include/strlib.h c/include/tokenscanner.h
	@echo "Build cmdscan.o"               
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/cmdscan.o -Ic/include c/src/cmdscan.c

build/$(PLATFORM)/obj/cmpfn.o: c/src/cmpfn.c c/include/cmpfn.h c/include/cslib.h c/include/generic.h \
             c/include/strlib.h
	@echo "Build cmpfn.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/cmpfn.o -Ic/include c/src/cmpfn.c

build/$(PLATFORM)/obj/cslib.o: c/src/cslib.c c/include/cslib.h c/include/exception.h
	@echo "Build cslib.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/cslib.o -Ic/include c/src/cslib.c

build/$(PLATFORM)/obj/exception.o: c/src/exception.c c/include/cmpfn.h c/include/cslib.h \
                 c/include/exception.h c/include/generic.h c/include/strlib.h \
                 c/include/unittest.h
	@echo "Build exception.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/exception.o -Ic/include c/src/exception.c

build/$(PLATFORM)/obj/filelib.o: c/src/filelib.c c/include/cmpfn.h c/include/cslib.h \
               c/include/exception.h c/include/filelib.h c/include/generic.h \
               c/include/iterator.h c/include/itertype.h c/include/strlib.h \
               c/include/unittest.h
	@echo "Build filelib.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/filelib.o -Ic/include c/src/filelib.c

build/$(PLATFORM)/obj/foreach.o: c/src/foreach.c c/include/cslib.h c/include/foreach.h \
               c/include/iterator.h
	@echo "Build foreach.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/foreach.o -Ic/include c/src/foreach.c

build/$(PLATFORM)/obj/generic.o: c/src/generic.c c/include/charset.h c/include/cmpfn.h \
               c/include/cslib.h c/include/exception.h c/include/generic.h \
               c/include/gevents.h c/include/gobjects.h c/include/gtimer.h \
               c/include/gtypes.h c/include/gwindow.h c/include/hashmap.h \
               c/include/iterator.h c/include/map.h c/include/pqueue.h \
               c/include/queue.h c/include/ref.h c/include/set.h c/include/stack.h \
               c/include/strbuf.h c/include/strlib.h c/include/vector.h
	@echo "Build generic.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/generic.o -Ic/include c/src/generic.c

build/$(PLATFORM)/obj/gevents.o: c/src/gevents.c c/include/cmpfn.h c/include/cslib.h \
               c/include/exception.h c/include/generic.h c/include/gevents.h \
               c/include/ginteractors.h c/include/gobjects.h c/include/gtimer.h \
               c/include/gtypes.h c/include/gwindow.h c/include/hashmap.h \
               c/include/iterator.h c/include/platform.h c/include/sound.h \
               c/include/strlib.h c/include/unittest.h c/include/vector.h
	@echo "Build gevents.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/gevents.o -Ic/include c/src/gevents.c

build/$(PLATFORM)/obj/gmath.o: c/src/gmath.c c/include/gmath.h
	@echo "Build gmath.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/gmath.o -Ic/include c/src/gmath.c

build/$(PLATFORM)/obj/gobjects.o: freeimage c/src/gobjects.c c/include/cmpfn.h c/include/cslib.h \
                c/include/generic.h c/include/gevents.h c/include/ginteractors.h \
                c/include/gmath.h c/include/gobjects.h c/include/gtimer.h \
                c/include/gtypes.h c/include/gwindow.h c/include/platform.h \
                c/include/sound.h c/include/vector.h  resources/FreeImage/Dist/FreeImage.h
	@echo "Build gobjects.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/gobjects.o -Ic/include -Iresources/FreeImage/Dist/ c/src/gobjects.c

build/$(PLATFORM)/obj/graph.o: c/src/graph.c c/include/cmpfn.h c/include/cslib.h \
             c/include/exception.h c/include/foreach.h c/include/generic.h \
             c/include/graph.h c/include/hashmap.h c/include/iterator.h \
             c/include/itertype.h c/include/set.h c/include/strlib.h \
             c/include/unittest.h
	@echo "Build graph.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/graph.o -Ic/include c/src/graph.c

build/$(PLATFORM)/obj/gtimer.o: c/src/gtimer.c c/include/cmpfn.h c/include/cslib.h \
              c/include/generic.h c/include/gevents.h c/include/ginteractors.h \
              c/include/gobjects.h c/include/gtimer.h c/include/gtypes.h \
              c/include/gwindow.h c/include/platform.h c/include/sound.h \
              c/include/vector.h
	@echo "Build gtimer.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/gtimer.o -Ic/include c/src/gtimer.c

build/$(PLATFORM)/obj/gtypes.o: c/src/gtypes.c c/include/cmpfn.h c/include/cslib.h \
              c/include/exception.h c/include/generic.h c/include/gtypes.h \
              c/include/unittest.h
	@echo "Build gtypes.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/gtypes.o -Ic/include c/src/gtypes.c

build/$(PLATFORM)/obj/gwindow.o: c/src/gwindow.c c/include/cmpfn.h c/include/cslib.h \
               c/include/generic.h c/include/gevents.h c/include/ginteractors.h \
               c/include/gmath.h c/include/gobjects.h c/include/gtimer.h \
               c/include/gtypes.h c/include/gwindow.h c/include/platform.h \
               c/include/sound.h c/include/vector.h
	@echo "Build gwindow.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/gwindow.o -Ic/include c/src/gwindow.c

build/$(PLATFORM)/obj/hashmap.o: c/src/hashmap.c c/include/cmpfn.h c/include/cslib.h \
               c/include/exception.h c/include/foreach.h c/include/generic.h \
               c/include/hashmap.h c/include/iterator.h c/include/itertype.h \
               c/include/strlib.h c/include/unittest.h
	@echo "Build hashmap.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/hashmap.o -Ic/include c/src/hashmap.c

build/$(PLATFORM)/obj/iterator.o: c/src/iterator.c c/include/cmpfn.h c/include/cslib.h \
                c/include/iterator.h c/include/itertype.h
	@echo "Build iterator.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/iterator.o -Ic/include c/src/iterator.c

build/$(PLATFORM)/obj/loadobj.o: c/src/loadobj.c c/include/cmpfn.h c/include/cslib.h \
               c/include/filelib.h c/include/generic.h c/include/iterator.h \
               c/include/loadobj.h c/include/strlib.h
	@echo "Build loadobj.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/loadobj.o -Ic/include c/src/loadobj.c

build/$(PLATFORM)/obj/map.o: c/src/map.c c/include/bst.h c/include/cmpfn.h c/include/cslib.h \
           c/include/exception.h c/include/foreach.h c/include/generic.h \
           c/include/iterator.h c/include/itertype.h c/include/map.h \
           c/include/strlib.h c/include/unittest.h
	@echo "Build map.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/map.o -Ic/include c/src/map.c

build/$(PLATFORM)/obj/options.o: c/src/options.c c/include/cmpfn.h c/include/cslib.h \
               c/include/exception.h c/include/generic.h c/include/hashmap.h \
               c/include/iterator.h c/include/options.h c/include/strlib.h \
               c/include/unittest.h c/include/vector.h
	@echo "Build options.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/options.o -Ic/include c/src/options.c

build/$(PLATFORM)/obj/platform.o: c/src/platform.c c/include/cmpfn.h c/include/cslib.h \
                c/include/filelib.h c/include/generic.h c/include/gevents.h \
                c/include/ginteractors.h c/include/gobjects.h c/include/gtimer.h \
                c/include/gtypes.h c/include/gwindow.h c/include/hashmap.h \
                c/include/iterator.h c/include/platform.h \
                c/include/private/tokenpatch.h c/include/queue.h \
                c/include/simpio.h c/include/sound.h c/include/strbuf.h \
                c/include/strlib.h c/include/tokenscanner.h c/include/vector.h
	@echo "Build platform.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/platform.o -Ic/include c/src/platform.c

build/$(PLATFORM)/obj/pqueue.o: c/src/pqueue.c c/include/cmpfn.h c/include/cslib.h \
              c/include/exception.h c/include/generic.h c/include/pqueue.h \
              c/include/unittest.h c/include/vector.h
	@echo "Build pqueue.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/pqueue.o -Ic/include c/src/pqueue.c

build/$(PLATFORM)/obj/queue.o: c/src/queue.c c/include/cmpfn.h c/include/cslib.h \
             c/include/exception.h c/include/generic.h c/include/queue.h \
             c/include/unittest.h
	@echo "Build queue.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/queue.o -Ic/include c/src/queue.c

build/$(PLATFORM)/obj/random.o: c/src/random.c c/include/cslib.h c/include/exception.h \
              c/include/private/randompatch.h c/include/random.h \
              c/include/unittest.h
	@echo "Build random.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/random.o -Ic/include c/src/random.c

build/$(PLATFORM)/obj/ref.o: c/src/ref.c c/include/cslib.h c/include/ref.h
	@echo "Build ref.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/ref.o -Ic/include c/src/ref.c

build/$(PLATFORM)/obj/set.o: c/src/set.c c/include/bst.h c/include/cmpfn.h c/include/cslib.h \
           c/include/exception.h c/include/foreach.h c/include/generic.h \
           c/include/iterator.h c/include/itertype.h c/include/map.h \
           c/include/set.h c/include/strlib.h c/include/unittest.h
	@echo "Build set.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/set.o -Ic/include c/src/set.c

build/$(PLATFORM)/obj/simpio.o: c/src/simpio.c c/include/cmpfn.h c/include/cslib.h \
              c/include/generic.h c/include/simpio.h c/include/strlib.h
	@echo "Build simpio.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/simpio.o -Ic/include c/src/simpio.c

build/$(PLATFORM)/obj/sound.o: c/src/sound.c c/include/cmpfn.h c/include/cslib.h c/include/generic.h \
             c/include/gevents.h c/include/ginteractors.h c/include/gobjects.h \
             c/include/gtimer.h c/include/gtypes.h c/include/gwindow.h \
             c/include/platform.h c/include/sound.h c/include/vector.h
	@echo "Build sound.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/sound.o -Ic/include c/src/sound.c

build/$(PLATFORM)/obj/stack.o: c/src/stack.c c/include/cmpfn.h c/include/cslib.h \
             c/include/exception.h c/include/generic.h c/include/stack.h \
             c/include/unittest.h
	@echo "Build stack.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/stack.o -Ic/include c/src/stack.c

build/$(PLATFORM)/obj/strbuf.o: c/src/strbuf.c c/include/cmpfn.h c/include/cslib.h \
              c/include/exception.h c/include/generic.h c/include/strbuf.h \
              c/include/strlib.h c/include/unittest.h
	@echo "Build strbuf.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/strbuf.o -Ic/include c/src/strbuf.c

build/$(PLATFORM)/obj/strlib.o: c/src/strlib.c c/include/cmpfn.h c/include/cslib.h \
              c/include/exception.h c/include/generic.h c/include/strlib.h \
              c/include/unittest.h
	@echo "Build strlib.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/strlib.o -Ic/include c/src/strlib.c

build/$(PLATFORM)/obj/tokenscanner.o: c/src/tokenscanner.c c/include/cmpfn.h c/include/cslib.h \
                    c/include/exception.h c/include/generic.h \
                    c/include/private/tokenpatch.h c/include/strbuf.h \
                    c/include/strlib.h c/include/tokenscanner.h \
                    c/include/unittest.h
	@echo "Build tokenscanner.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/tokenscanner.o -Ic/include c/src/tokenscanner.c

build/$(PLATFORM)/obj/unittest.o: c/src/unittest.c c/include/cmpfn.h c/include/cslib.h \
                c/include/exception.h c/include/generic.h c/include/strlib.h \
                c/include/unittest.h
	@echo "Build unittest.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/unittest.o -Ic/include c/src/unittest.c

build/$(PLATFORM)/obj/unixfile.o: c/src/unixfile.c c/include/cmpfn.h c/include/cslib.h \
                c/include/filelib.h c/include/generic.h c/include/iterator.h \
                c/include/strlib.h c/include/vector.h
	@echo "Build unixfile.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/unixfile.o -Ic/include c/src/unixfile.c

build/$(PLATFORM)/obj/winfile.o: c/src/unixfile.c c/include/cmpfn.h c/include/cslib.h \
                c/include/filelib.h c/include/generic.h c/include/iterator.h \
                c/include/strlib.h c/include/vector.h
	@echo "Build winfile.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/winfile.o -Ic/include c/src/winfile.c

build/$(PLATFORM)/obj/vector.o: c/src/vector.c c/include/cmpfn.h c/include/cslib.h \
              c/include/exception.h c/include/generic.h c/include/iterator.h \
              c/include/itertype.h c/include/strlib.h c/include/unittest.h \
              c/include/vector.h
	@echo "Build vector.o"
	@$(CC) $(CFLAGS) -D$(PLATFORM) -c -o build/$(PLATFORM)/obj/vector.o -Ic/include c/src/vector.c


# ***************************************************************
# Entry to reconstruct the library archive

build/$(PLATFORM)/lib/libcs.a: $(OBJECTS) resources/FreeImage/Dist/libFreeImage.a
	@echo "Build libcs.a"
	@-rm -f build/$(PLATFORM)/lib/libcs.a
	@-rm -f build/$(PLATFORM)/lib/libfreeimage.a
	@ar cr build/$(PLATFORM)/lib/libcs.a \
			 $(OBJECTS) 

	@ranlib build/$(PLATFORM)/lib/libcs.a 
	@cp -r c/include build/$(PLATFORM)/
	@cp resources/FreeImage/Dist/FreeImage.h build/$(PLATFORM)/include/
	@cp resources/FreeImage/Dist/libfreeimage.a build/$(PLATFORM)/lib/

# ***************************************************************
# Entry to build Test program

build/$(PLATFORM)/obj/TestStanfordCSLib.o: c/tests/TestStanfordCSLib.c c/include/cslib.h \
	c/include/strlib.h c/include/unittest.h
	@echo "Build TestStanfordCSLib.o"
	@$(CC) $(CFLAGS) -c -o build/$(PLATFORM)/obj/TestStanfordCSLib.o -Ic/include \
            c/tests/TestStanfordCSLib.c 

TestStanfordCSLib: $(TESTOBJECTS) resources/FreeImage/Dist/libFreeImage.a build/$(PLATFORM)/lib/libcs.a
	@echo "Build TestStanfordCSLib"
	@$(CC) $(CFLAGS) -o build/$(PLATFORM)/tests/TestStanfordCSLib $(TESTOBJECTS) -Lbuild/$(PLATFORM)/lib -lcs -lm $(LDLIBS)

# ***************************************************************
# Entry to build Java Back End

$(JAR): stanford/spl/JavaBackEnd.class
	@echo "Build spl.jar"
	@cp java/lib/acm.jar build/$(PLATFORM)/lib/spl.jar
	@(cd build/$(PLATFORM)/classes; jar ufm ../lib/spl.jar ../../../java/include/JBEManifest.txt \
		`find stanford -name '*.class'`)
#	@echo jar cf build/$(PLATFORM)/lib/spl.jar . . .

stanford/spl/JavaBackEnd.class: java/src/stanford/spl/*.java
	@echo "Build JavaBackEnd.class"
	@javac -d build/$(PLATFORM)/classes -classpath java/lib/acm.jar -sourcepath java/src \
		java/src/stanford/spl/JavaBackEnd.java


# ***************************************************************
# Entry to install the library

install: build/$(PLATFORM)/lib/libcs.a $(JAR)
	rm -rf /usr/local/include/spl
	cp -r build/$(PLATFORM)/include /usr/local/include/spl
	chmod -R a+rX /usr/local/include/spl
	cp build/$(PLATFORM)/lib/{libcs.a,spl.jar} /usr/local/lib/
	chmod -R a+r /usr/local/lib/{libcs.a,spl.jar}

# ***************************************************************
# Entry to build the examples

examples: build/$(PLATFORM)/lib/libcs.a $(JAR)
	@echo "Build Examples"
	@cp build/$(PLATFORM)/lib/spl.jar c/examples/
	@make -C c/examples

# ***************************************************************
# Entry to bring the package up to date
#    The "make all" entry should be the first real entry

all: $(RESOURCES) $(BUILD) $(OBJECTS) $(LIBRARIES) $(TESTS) $(JAR) $(EXAMPLES)

lib: $(RESOURCES) $(BUILD) $(OBJECTS) $(LIBRARIES) $(TESTS) $(JAR)

starterprojects: lib
	@echo "Build StarterProjects"
	@rm -rf StarterProjects
	@mkdir StarterProjects
	@cp -r ide/clion StarterProjects/clion
	@cp -r ide/codeblocks StarterProjects/codeblocks
	@cp -r ide/makefile StarterProjects/makefile
	@echo "Build StarterProject for Clion on Windows"
	@cp ide/src/HelloGraphics.c StarterProjects/clion/windows
	@cp -r build/$(PLATFORM)/lib StarterProjects/clion/windows/lib
	@cp -r build/$(PLATFORM)/include StarterProjects/clion/windows/include
	@echo "Build StarterProject for Clion on Linux"
	@cp ide/src/HelloGraphics.c StarterProjects/clion/linux
	@cp -r build/$(PLATFORM)/lib StarterProjects/clion/linux/lib
	@cp -r build/$(PLATFORM)/include StarterProjects/clion/linux/include
	@echo "Build StarterProject for Clion on MacOS"
	@cp ide/src/HelloGraphics.c StarterProjects/clion/macos
	@cp -r build/$(PLATFORM)/lib StarterProjects/clion/macos/lib
	@cp -r build/$(PLATFORM)/include StarterProjects/clion/macos/include
	@echo "Build StarterProject for CodeBlocks on Windows"
	@cp ide/src/HelloGraphics.c StarterProjects/codeblocks/windows
	@cp -r build/$(PLATFORM)/lib StarterProjects/codeblocks/windows/lib
	@cp -r build/$(PLATFORM)/include StarterProjects/codeblocks/windows/include
	@echo "Build StarterProject for CodeBlocks on Linux"
	@cp ide/src/HelloGraphics.c StarterProjects/codeblocks/linux
	@cp -r build/$(PLATFORM)/lib StarterProjects/codeblocks/linux/lib
	@cp -r build/$(PLATFORM)/include StarterProjects/codeblocks/linux/include
	@echo "Build StarterProject for CodeBlocks on MacOS"
	@cp ide/src/HelloGraphics.c StarterProjects/codeblocks/macos
	@cp -r build/$(PLATFORM)/lib StarterProjects/codeblocks/macos/lib
	@cp -r build/$(PLATFORM)/include StarterProjects/codeblocks/macos/include
	@echo "Build StarterProject for Makefile Project"
	@cp ide/src/HelloGraphics.c StarterProjects/makefile
	@cp -r build/$(PLATFORM)/lib StarterProjects/makefile/lib
	@cp -r build/$(PLATFORM)/include StarterProjects/makefile/include
	@echo "Check the StarterProjects folder"

clion: lib
	@echo "Build StarterProject for Clion on Linux";
	@rm -rf StarterProject
	@cp -r ide/clion/linux StarterProject
	@cp -r build/$(PLATFORM)/lib StarterProject
	@cp -r build/$(PLATFORM)/include StarterProject
	@cp ide/src/HelloGraphics.c StarterProject
	@echo "Check the StarterProject folder"

codeblocks: lib
	@echo "Build StarterProject for CodeBlocks on Linux";
	@rm -rf StarterProject
	@cp -r ide/codeblocks/linux StarterProject
	@cp -r build/$(PLATFORM)/lib StarterProject/lib
	@cp -r build/$(PLATFORM)/include StarterProject/include
	@cp ide/src/HelloGraphics.c StarterProject
	@echo "Check the StarterProject folder"

makefile: lib
	@echo "Build StarterProject for Makefile Project";
	@rm -rf StarterProject
	@cp -r ide/makefile StarterProject
	@cp -r build/$(PLATFORM)/lib StarterProject
	@cp -r build/$(PLATFORM)/include StarterProject
	@cp ide/src/HelloGraphics.c StarterProject
	@echo "Check the StarterProject folder"

# ***************************************************************
# Standard entries to remove files from the directories
#    tidy    -- eliminate unwanted files
#    scratch -- delete derived files in preparation for rebuild

tidy: examples-tidy
	@echo "Clean Project Directory"
	@rm -f `find . -name ',*' -o -name '.,*' -o -name '*~'`
	@rm -f `find . -name '*.tmp' -o -name '*.err'`
	@rm -f `find . -name core -o -name a.out`
	@rm -rf build/classes
	@rm -rf build/obj

examples-tidy:
	@rm -f c/examples/*.o
	@rm -f c/examples/*.exe

scratch clean: tidy freeimage_clean
	@rm -f -r $(BUILD) $(OBJECTS) $(LIBRARIES) $(TESTS) $(TESTOBJECTS) $(PROJECT)
	@echo "Cleaning Done"

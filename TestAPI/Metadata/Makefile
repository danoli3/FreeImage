CXXFLAGS ?= -O2 -g

.PHONY: check clean

check: taglib-test
	./taglib-test

taglib-test: TagLibTest.cpp ../../Source/Metadata/TagLib.cpp ../../Source/Metadata/FreeImageTag.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -std=c++98 -I../../Source TagLibTest.cpp $(LDFLAGS) -o $@

clean:
	$(RM) taglib-test

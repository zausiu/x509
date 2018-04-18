#######################################################
## created by kamuszhou@tencent.com on March 13
#######################################################

.PHONY: clean all

sources := $(shell find . -name "*.cc")
objects := $(subst .cc,.o,$(sources))
objects_no_main := $(filter-out %main.o, $(objects))
dependencies := $(subst .cc,.d,$(sources))

libqos_dir := ../libqos-client64-4.1.1_64bit
include_dirs := $(libqos_dir)

CPPFLAGS += -I. $(addprefix -I,$(include_dirs))
CXXFLAGS += -fPIC
LINKFLAGS += -L$(libqos_dir) -lpthread

libqos_lib_path := $(libqos_dir)/libqos_client.a

ifneq "$(MAKECMDGOAL)" "clean"
	-include $(dependencies)
endif

define make-depend
	g++ -MM -MF $3 -MT $2 $(CPPFLAGS) $(TARGET_ARCH) $1
endef

all: l5wrapper.so test

%.o: %.cpp
	$(call make-depend, $<,$@,$(subst .cpp,.d,$<))
	$(COMPILE.C) $(OUTPUT_OPTION) $<

l5wrapper.so: $(objects_no_main) 
	g++ -shared -o $@ $^ $(libqos_lib_path)

test: $(objects)
	g++ -o $@ $(objects) $(libqos_lib_path) $(LINKFLAGS)

clean: 
	-rm $(objects) $(dependencies) l5wrapper.so test

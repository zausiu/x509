#created by kamuszhou@tencent.com

root := ../
public_dir := ../public/
boost_home := /data1/home/kamuszhou/
#boost_home2 := /data/home/rubinwang/local/
lib := $(HOME)/lib
mysql_lib := /data1/home/kamuszhou/lib/ /usr/local/mysql/lib/ /data/kamus/lib /usr/lib64/mysql/
bin := $(root)bin/

other_source_dirs += 

apppath := $(bin)$(projname)
sopath := $(bin)$(soname)
archive_path := $(bin)$(archive)
sources := $(shell find . -name "*.cpp") $(shell find $(public_dir) -type f \( -path "*spdlog*" -prune -o -name "*.cpp" -print \) ) $(wildcard ../mylogger/*.cpp) $(filter-out %main.cpp, $(wildcard $(addsuffix *.cpp,$(other_source_dirs))))
objects := $(subst .cpp,.o,$(sources))
objects_nomain := $(filter-out %main.o, $(objects))
dependencies := $(subst .cpp,.d,$(sources))
#lib_inc_dirs := $(notdir $(patsubst %/,%,$(shell ls -d ../../lib/*/)))
#include_dirs += . $(public_dir) $(addprefix $(root)inc/,$(lib_inc_dirs))
include_dirs += $(public_dir) ../mylogger ../gleaner $(HOME)/local/include /data/kamus/include

CXXFLAGS += -fPIC 
#CPPFLAGS += $(addprefix -I,$(include_dirs)) -I$(boost_home)include -I$(boost_home2)include
CPPFLAGS += -Wattributes -Wdeprecated-declarations -I. $(addprefix -I,$(include_dirs)) -I$(boost_home)include -I../public/spdlog/include

ifndef OVERRIDE_LINKFLAGS 
LINKFLAGS += -L$(lib) $(addprefix -L,$(mysql_lib)) -lmysqlcppconn-static -lmysqlclient -lcrypto -L$(boost_home)lib -lboost_filesystem -lboost_system -lboost_regex -lboost_thread -lboost_program_options -lboost_date_time -lboost_chrono -lm -lpthread -lrt -ldl -ldl -lutil -ldl -lutil -lz -lqos_client 
endif

vpath %.cpp . $(public_dir)
vpath %.h $(include_dirs)

.PHONY: clean $(projname) $(soname)

all: $(projname) $(soname) $(archive)

ifdef projname
$(projname): $(apppath)

$(apppath): $(objects)
	test -d $(bin) || mkdir -p $(bin)
	g++ -o $@ $^ $(LINKFLAGS)
	md5sum $@
endif

ifdef soname
$(soname): $(sopath)

$(sopath): $(objects_nomain)
	test -d $(bin) || mkdir -p $(bin)
	g++ -shared -o $@ $^ $(LINKFLAGS)
	md5sum $@
endif

ifdef archive
$(archive): $(archive_path)

$(archive_path): $(objects_nomain)
	test -d $(bin) || mkdir -p $(bin)
	ar rv -o $@ $^ 
	md5sum $@
endif

ifneq "$(MAKECMDGOAL)" "clean"
  -include $(dependencies)
endif

#$(call make-depend,source-file,object-file,depend-file) 
# -MM option causes g++ to omit "system" headers from the prerequisites list.
# This option implies -E which will stop after the preprocessing stage; do not
# run the compiler proper. 
# -MF option specifies the dependecy filename.
# -MT target change the target of the rule emitted by dependency generation.
define make-depend
   g++ -MM -MF $3 -MT $2 $(CPPFLAGS) $(CXXFLAGS) $(TARGET_ARCH) $1
endef

%.o : %.cpp
	$(call make-depend, $<,$@,$(subst .cpp,.d,$<))
	$(COMPILE.C) $(OUTPUT_OPTION) $<
#	g++ $(CXXFLAGS) $(CPPFLAGS) $< # lacks -c -o 

clean :
	rm -rf *.o *.d $(apppath) *.log $(public_dir)*.o $(public_dir)*.d $(dependencies) $(objects)
	-find . -name *.o -delete
	-find . -name *.d -delete

	
ifneq "$(MAKECMDGOAL)" "clean"
  -include $(dependencies)
endif

#$(call make-depend,source-file,object-file,depend-file) 
# -MM option causes g++ to omit "system" headers from the prerequisites list.
# This option implies -E which will stop after the preprocessing stage; do not
# run the compiler proper. 
# -MF option specifies the dependecy filename.
# -MT target change the target of the rule emitted by dependency generation.
define make-depend
   g++ -MM -MF $3 -MT $2 $(CPPFLAGS) $(CXXFLAGS) $(TARGET_ARCH) $1
endef

%.o : %.cpp
	$(call make-depend, $<,$@,$(subst .cpp,.d,$<))
	$(COMPILE.C) $(OUTPUT_OPTION) $<
#	g++ $(CXXFLAGS) $(CPPFLAGS) $< # lacks -c -o 

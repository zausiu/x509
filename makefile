################################################
# created by kamuszhou@tencent.com
################################################

# list all sub-directories
dirs := $(shell ls -d */)
projs := $(filter-out cert/ scripts/ spdlog/ public/ bin/, $(dirs))

.PHONY: all $(projs)

all: $(projs) 

$(projs):
	$(MAKE) --directory=$@ $(TARGET)

clean:
	for d in $(projs);          \
	do                                      \
		$(MAKE) --directory=$$d clean;  \
	done

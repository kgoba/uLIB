SUBDIRS = examples/test
SUBCLEAN = $(addsuffix .clean, $(SUBDIRS))

.PHONY: all $(SUBDIRS) clean $(SUBCLEAN)

all: $(SUBDIRS)

$(SUBDIRS):
	@echo "Building $@..."
	@$(MAKE) -C $@

clean: $(SUBCLEAN)

$(SUBCLEAN): %.clean:
	@echo "Cleaning $*..."
	@$(MAKE) -C $* clean


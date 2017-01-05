include env.mk


ifeq ($(prefix),)
    BIN_DIR = $(shell pwd)
else
    BIN_DIR = $(prefix)/lib
endif

ifeq ($(build),)
    BUILD_DIR = $(shell pwd)/build
else
    BUILD_DIR = $(build)/ylog
endif

vpath %.o $(BUILD_DIR)


OBJS = ylog.o

LIB = $(BIN_DIR)/libylog.a

.PHONY: all
all: $(LIB)

$(LIB): $(OBJS)
	@$(MKDIR) $(BIN_DIR)
	@cd $(BUILD_DIR)  &&  $(AR) rcs $(LIB) $(OBJS)  && echo -e "  AR	$(LIB) Success."

%.o: %.cpp
	@$(MKDIR) $(BUILD_DIR)
	@echo -e "  CXX		$<"  &&  $(CXX) -c $(CXXFLAGS) $< -o $(BUILD_DIR)/$@

%.d: %.cpp
	@set -e; $(RM) $@; $(CXX) -MM $< $(CXXFLAGS) > $@.$$$$; \
	    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	    $(RM) $@.$$$$

-include $(OBJS:.o=.d)


.PHONY: clean
clean:
	@echo -e "  Clean		ylog"  &&  $(RM) $(LIB) $(BUILD_DIR) *.d *.d.*

include env.mk


ifeq ($(prefix),)
    BIN_DIR = $(shell pwd)
else
    BIN_DIR = $(prefix)/lib
endif

ifeq ($(build),)
    BUILD_DIR = $(shell pwd)
else
    BUILD_DIR = $(build)/ylog
endif


OBJS = $(BUILD_DIR)/ylog.o

LIB = $(BIN_DIR)/libylog.a

.PHONY: all
all: $(LIB)

$(LIB): $(OBJS)
	@$(MKDIR) $(BIN_DIR)
	@$(AR) rcs $(LIB) $(OBJS)  && echo -e "  AR	$(LIB) Success."

$(BUILD_DIR)/%.o: %.cpp
	@$(MKDIR) $(BUILD_DIR)
	@echo -e "  CXX		$<"  &&  $(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	@echo -e "  Clean		ylog"  &&  $(RM) $(LIB) $(OBJS)

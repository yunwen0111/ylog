include env.mk


ifeq ($(prefix),)
    BIN_DIR = $(shell pwd)
else
    BIN_DIR = $(prefix)/lib
endif


OBJS = ylog.o

LIB = $(BIN_DIR)/libylog.a

.PHONY: all
all: $(LIB)

$(LIB): $(OBJS)
	@$(MKDIR) $(BIN_DIR)
	@echo -e "\033[31m  AR	$(LIB)  <<== [ $(OBJS) ]\033[00m"\
                &&  $(AR) rcs $(LIB) $(OBJS)
	@echo -e "\033[34m  AR $(LIB) Success.\033[00m"

%.o: %.cpp
	@echo -e "\033[31m  CXX	$<\033[00m"  &&  $(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	@echo -e "\033[31m  RM	$(LIB) $(OBJS)\033[00m"\
                &&  $(RM) $(LIB) $(OBJS)

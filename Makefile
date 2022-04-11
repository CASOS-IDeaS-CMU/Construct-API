#Author: Stephen Dipple
#Last Updated: 7/29/21

#This makefile will grab all cpp files in all include files, source dir, and
#any additional subdirectories in the source dir. This does not do a recursive for cpp files
#CXXFLAGS and OUTPUT_EXECTUABLE can be defined via the command line, but
#default to "" and "construct" respectively.
#Ex.
#make OUTPUT_EXECUTABLE=my_exe_name
#make CXXFLAGS=-DEBUG

API_CONFIG ?= Debug
override API_OBJECT_OPTIONS += -c -fPIC

#To raise assertions and process other debug stuff use -DEBUG
#To have the simulation run in quiet mode where
#no output is sent to the terminal window use -QUIET
CXXFLAGS ?=

#location of main.cpp
#location can be relative or absolute
SRC_DIR = .

#additional subdirectories in the source dir
override SUB_DIR += $(SRC_DIR)/Construct_Library

API_FILE ?= Supp_Library

#Add all includes here
#make sure there aren't any forward slashes before or after the dir
#location is relative to makefile
#the include dir needs to be in a subdir of the makefile
#in order for the the cpp files to be found
CONSTRUCT_HEADER_DIR = $(SRC_DIR)/Construct_Library
override INC += $(SRC_DIR) $(CONSTRUCT_HEADER_DIR)

#All object files get moved to this dir
#The dir is created if not already present
#location is relative to makefile

CXX = g++ -std=c++20
# end user defined variables



######################################################################
#    ATTENTION
#			You should not have to modify anything below this line
#			If you do, please notify Michael Kowalchuck at kf3cr@cs.cmu.edu
######################################################################

.PHONY: all export_unresolved clean FORCE


ifeq ($(API_CONFIG),Debug)
override CXXFLAGS += DEBUG 
else ifeq ($(API_CONFIG),Quiet)
override CXXFLAGS += QUIET
endif

API_OUT ?= $(SRC_DIR)/$(UNAME_S)/$(API_CONFIG)

API_LIB ?= constructAPI
EXE_LIB ?= construct

#this makefile can work with either linux or mac
#this handles the differences in extensions
UNAME_S = $(shell uname -s)

ifeq ($(UNAME_S),Linux)
API_LINKER_OPTION = -shared
LIB_EXT = so 
endif

ifeq ($(UNAME_S),Darwin)
API_LINKER_OPTION = -dynamiclib
LIB_EXT = dylib
endif

override MACROS = $(patsubst %, -D%, $(CXXFLAGS))
override INC_DIR += $(patsubst %, -I%, $(INC))

#collects all cpp and header files in directed dirs
override SRC_CPP = $(notdir $(foreach dir, $(INC), $(wildcard $(dir)/*.cpp)))
override SRC_HED = $(foreach dir, $(INC), $(wildcard $(dir)/*.h))

#convert to a list of object files that should live in the out dir
override SRC_OBJ = $(patsubst %.cpp, $(API_OUT)/%.o, $(SRC_CPP))
#$(shell scl enable devtoolset-10 -- bash)

#the out dir is created first if it does not exist
#then it checks for every object file in the out dir

all: $(SRC_OBJ) $(SRC_HED) $(API_OUT)/lib$(EXE_LIB).a
	$(CXX) $(API_LINKER_OPTION) -L$(API_OUT) -l$(EXE_LIB) -o $(API_OUT)/lib$(API_LIB).$(LIB_EXT) $(SRC_OBJ) $(MACROS)
	@echo
	@echo Shared Library successfully created
	@echo

export_unresolved: $(API_OUT)/$(API_FILE).o
	$(CXX) $(API_LINKER_OPTIONS) -o $(API_OUT)/lib$(API_LIB).$(LIB_EXT) $(API_OUT)/$(API_FILE).o $(MARCOS)


#this is needed to do the recursive search for the specified cpp and h files in build/%.o
.SECONDEXPANSION:

$(API_OUT)/%.o: $$(shell find .. -name $$*.cpp)
	mkdir -p $(API_OUT)
	$(CXX) $(API_OBJECT_OPTIONS) $(INC_DIR) $< -o $@ $(MACROS)

	
clean:
	rm -f $(OUT)/*.o 


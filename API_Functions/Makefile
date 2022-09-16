#Author: Stephen Dipple
#Last Updated: 4/29/22

# This makefile is used to create shared libraries used in the Construct API.
# By default this makefile will produce the debug version of the shared library.
# To change this configuration use one of the following targets.

# debug	  -Same as default
# quiet	  -Compiles the library with the QUIET and NDEBUG preprocessor flag
# release -Compiles the library with the NDEBUG preprocessor flag

# A complimentary set of targets is used to clean up their respective configurations
# clean_debug
# clean_quiet
# clean_release

# These targets/configurations automatically store intermidiary and output files in
# directories determined by the operating system and configuration.
# Output file targets use a wildcard match with the path for that file.
# This allows output files to be sent to other directories when created.

# The first section of the makefile can be modified by the user if necessary.
# The second section are contents that should not be modified as targets expect
# certain qualities of the variables they call.
# If a problem occurs, leave a comment on the GitHub repo, 
# contact the lead developer of Construct, 
# or post your issue to the ORA Google Group https://groups.google.com/g/ORA-google-group.

# What can you expect this makefile to do for you?
# This makefile automatically gathers up all source and header files from paths
# in the INC variable.
# To add additional directories, declare them in the ADD_INC variable,
# which appends the include directories list.
# In addition, the Makefile will add any preprocessor flags from the CXXFLAGS variable.
# These are added to configuration dependent preprocessor flags during compliation.
# The elements of this variable should be exactly as they appear in the source files.
# Libraries can be included via the EXE_OPTIONS and OBJECT_OPTIONS variables
# These are used verbatim in the compiler so be sure to use -l -L or -I to include paths/files 
# The Construct exes require a specific naming convention, which is also handled by this Makefile.






#################################
# Section 1			#
#################################

# The default configuration is Debug
# This variable controls the path files are stored in unless otherwise directed
# Ex. /path/to/makefile/Linux/Debug/
CONFIG ?=

# Allows for additional options to be given to the creation of object files.
# Options that are added to this variable in section are "-c -fPIC"
OBJECT_OPTIONS ?=

# Allows for additional options to be given to the linker process.
LINKER_OPTIONS ?=

# This variable adds preprocessor flags to the comiliation process.
# Elements do not need -D added infront of them and should appear 
# exactly as the flags are used in the source files.
# Additional flags are added based on the configuration in section 2.
CXXFLAGS ?=

# Root directory for files
SRC_DIR := .

# Defines any additional directories that should be included
# All cpp files included in these directories are compiled.
# The base set of include directories are defined in Section 2.
ADD_INC ?=

# The location of the Construct Header
CONSTRUCT_HEADER_DIR = $(SRC_DIR)/../include $(SRC_DIR)/../include/Construct_Models

# The location of the static library that provides definitions for the declarations in the Construct Header
IMPORT_LIB_DIR = $(CONSTRUCT_HEADER_DIR)/$(UNAME_S)/$(CONFIG)


CXX = g++ -g -std=c++20
# end user defined variables













# No edits should be required in the following section
# If you feel that is not the case, contact the lead Construct developer

#################################
# Section 2			#
#################################

.PHONY: clean debug quiet release clean_debug clean_quiet clean_release FORCE

override INC := $(SRC_DIR) $(CONSTRUCT_HEADER_DIR)

debug: export CONFIG = Debug
debug: export CONFIG_FLAGS = DEBUG

# The variables set in the target are local variables.
# While the variables will be properly evaluated in commands,
# target prerequisites are evaluated before targets are done.
# This means we have to export the local variables and rerun make.
debug: 
	@$(MAKE) $(API_OUT)

# the quiet configuration will silence any runtime console output, but will still output errors
quiet: export CONFIG = Quiet
quiet: export CONFIG_FLAGS = QUIET NDEBUG

# You can't have a command in a target when defining a local variable.
quiet:
	@$(MAKE) $(API_OUT)

# the release version removes assertions and other checks once debugging is finished
release: export CONFIG = Release
release: export CONFIG_FLAGS = NDEBUG
release:
	@$(MAKE) $(API_OUT)




clean_debug: export CONFIG = Debug
clean_debug:
	@$(MAKE) clean

clean_quiet: export CONFIG = Quiet
clean_quiet:
	@$(MAKE) clean	

clean_release: export CONFIG = Release
clean_release:
	@$(MAKE) clean


# Directory that holds the corresponding library and exe
# Also holds intermediary object files
API_OUT_DIR ?= $(SRC_DIR)/$(UNAME_S)/$(CONFIG)
API_OUT = $(API_OUT_DIR)/lib$(API_LIB).$(LIB_EXT)

API_LIB ?= constructAPI
IMPORT_LIB ?= construct

#this makefile can work with either linux or mac
#this handles the differences in extensions
UNAME_S = $(shell uname -s)

# figures out the correct options and extensions for the operating system
ifeq ($(UNAME_S),Linux)
SHARED_OPTION = -shared
LIB_EXT = so 
endif

ifeq ($(UNAME_S),Darwin)
SHARED_OPTION = -dynamiclib
LIB_EXT = dylib
endif

# Adds -D to the beginning of each preprocessor flag
override MACROS = $(patsubst %, -D%, $(CXXFLAGS) $(CONFIG_FLAGS))

# Adds -I to the beginning of each include directory
override INC_DIR += $(patsubst %, -I%, $(INC) $(ADD_INC))

# collects all cpp and header files in directed dirs
override SRC_CPP = $(notdir $(foreach dir, $(INC) $(ADD_INC), $(wildcard $(dir)/*.cpp)))
override SRC_HED = $(foreach dir, $(INC) $(ADD_INC), $(wildcard $(dir)/*.h))

# convert to a list of object files that should live in the out dir
override SRC_OBJ = $(patsubst %.cpp, $(API_OUT_DIR)/%.o, $(SRC_CPP))

# Combines all the options required to create the shared library.
# This included the -shared/-dynamiclib option, the Construct shared library and its dir.
override SHARED_LIB_OPTIONS := $(SHARED_OPTION) -L$(IMPORT_LIB_DIR) -l$(IMPORT_LIB) 


# creates the shared library for the API
%/lib$(API_LIB).$(LIB_EXT): $(SRC_OBJ) $(SRC_HED) $(IMPORT_LIB_DIR)/lib$(IMPORT_LIB).a
	$(CXX) $(LINKER_OPTIONS) $(SRC_OBJ) $(SHARED_LIB_OPTIONS) -o $@ $(MACROS)
	

#this is needed to do the recursive search for the specified cpp and h files in build/%.o
.SECONDEXPANSION:

.SECONDARY: $(SRC_OBJ)

# because each object file is in a different location that the the corresponding cpp file
# we have to search for the cpp file which is what this shell command does
# it only searches files in this directory so as to make sure other files aren't accidently used
$(API_OUT_DIR)/%.o: $$(shell find .. -name $$*.cpp)
	$(CXX) -c -fPIC $(OBJECT_OPTIONS) $(INC_DIR) $< -o $@ $(MACROS)


	
clean:
	-rm -f $(API_OUT_DIR)/*.o 
	-rm $(API_OUT_DIR)/lib$(API_LIB).$(LIB_EXT)



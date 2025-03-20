#Author: Stephen Dipple
#Last Updated: 01/27/25

# This makefile is used to create an executable using the Construct API.
# By default this makefile will produce the debug version of the executable.
# To change this configuration use one of the following targets.

# debug	  -Same as default
# quiet	  -Compiles the executable with the QUIET and NDEBUG preprocessor flag
# release -Compiles the executable with the NDEBUG preprocessor flag

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
# Libraries can be included via the EXE_OPTIONS and OBJECT_OPTIONS variables.
# These are used verbatim in the compiler so be sure to use -l -L or -I to include paths/files.






#################################
# Section 1			#
#################################

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
INCLUDE = $(CONSTRUCT_HEADER_DIR) $(CONSTRUCT_MODELS_DIR) 

# The location of the Construct Header
CONSTRUCT_HEADER_DIR = $(SRC_DIR)/../include

# The location of the static library that provides definitions for the declarations in the Construct Header
IMPORT_LIB_DIR = $(CONSTRUCT_HEADER_DIR)/$(UNAME_S)

# The location of the Construct Models
CONSTRUCT_MODELS_DIR = $(CONSTRUCT_HEADER_DIR)/Construct_Models

CXX = g++ -g -std=c++20
# end user defined variables

EXCLUDE_CPPS ?=

API_OUT_DIR ?= $(SRC_DIR)/$(UNAME_S)

#lib always gets added as a prefix to libraries and the appropriate extension is added for the operating system
API_LIB ?= constructAPI








# No edits should be required in the following section
# If you feel that is not the case, contact the lead Construct developer

#################################
# Section 2			#
#################################

CONFIGS = Debug Quiet Release
config_Debug = -DDEBUG
config_Quiet = -DNDEBUG -DQUIET
config_Release = -DNDEBUG

.PHONY: clean CONFIGS


# Directory that holds the corresponding library and exe
# Also holds intermediary object files

API_OUT = $(API_OUT_DIR)/lib$(API_LIB).$(LIB_EXT)

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

SRC_CPP := $(foreach dir,$(INCLUDE),$(shell find $(dir) -maxdepth 1 -type f -name '*.cpp'))
SRC_CPP := $(filter-out $(EXCLUDE_CPPS),$(SRC_CPP))
SRC_OBJ := $(notdir $(SRC_CPP:.cpp=.o))

override MACROS = $(patsubst %, -D%, $(CXXFLAGS))

define build_rules
$(1): $(API_OUT_DIR)/$(1)/lib$(API_LIB).$(LIB_EXT)

$(API_OUT_DIR)/$(1)/lib$(API_LIB).$(LIB_EXT): $(patsubst %, $(API_OUT_DIR)/$(1)/%, $(SRC_OBJ))
	@$(CXX) $(LINKER_OPTIONS) $(patsubst %, $(API_OUT_DIR)/$(1)/%, $(SRC_OBJ)) $(SHARED_OPTION) -o $(API_OUT_DIR)/$(1)/lib$(API_LIB).$(LIB_EXT) \
	$(config_$(1)) $(MACROS) -L$(IMPORT_LIB_DIR)/$(1) -l$(IMPORT_LIB_DIR)/$(1)/lib$(IMPORT_LIB).$(LIB_EXT)
endef
$(foreach dir, $(CONFIGS), $(eval $(call build_rules,$(dir))))




OBJ_BUILD = $(CXX) -c $(OBJECT_OPTIONS) $(patsubst %, -I%, $(INCLUDE)) $(patsubst %, -D%, $(CXXFLAGS))

define obj_rule
$(EXE_OUT_DIR)/$(1)/$(notdir $(2:.cpp=.o)): $(2)
	@echo "Compiling $(notdir $(2:.cpp= ))"
	@$(OBJ_BUILD) $(2) -o $(EXE_OUT_DIR)/$(1)/$(notdir $(2:.cpp=.o)) $(config_$(1)) 
endef
$(foreach dir, $(CONFIGS) gui, $(foreach file, $(SRC_CPP), $(eval $(call obj_rule,$(dir),$(file)))))



	
clean:
	-rm -f $(API_OUT_DIR)/**/*.o 
	-rm $(API_OUT_DIR)/**/lib$(API_LIB).$(LIB_EXT)



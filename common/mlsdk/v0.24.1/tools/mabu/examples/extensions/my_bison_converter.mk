
# This invokes bison to create a parser file.
#
# Useful variables are:
#
#   PROJECT (basename of current project, with any spaces converted to '_')
#     $(PROJECT)_BASE (base directory of this project)
#     $(PROJECT)_SRCS (paths to all the incoming sources from SRCS)
#     $(PROJECT)_OUTPUT (output directory for this build target)
#     $(PROJECT)_OUTPUT_FILE (main output file for this build target)
#   ECHO (precede commands with this, for '-v')
#   INFO (precede commands with this to echo build status)
#   HOST (running host, 'win', 'osx', 'linux')
#   SPEC (full build specification)
#
# (note that $($(PROJECT)_BASE) should be included to avoid
# conflicting with similar patterns from other projects built simultaneously.)

BISON=bison

# get the bison sources that apply to this project
BISON_SRCS=$(filter %.y,$($(PROJECT)_SRCS))

# presumed base of all bison source files
BISON_BASE=$($(PROJECT)_BASE)

# this is where we want to generate files
BISON_OUT=$($(PROJECT)_BASE)/generated

# deduce the output files, for the clean rule
BISON_CPP_SRCS = $(patsubst $(BISON_BASE)/%.y,$(BISON_OUT)/%.c,$(BISON_SRCS))
BISON_CPP_HEADERS = $(patsubst $(BISON_BASE)/%.y,$(BISON_OUT)/%.h,$(BISON_SRCS))

$(PROJECT)-bison-info:
	$(SPAM) "bison sources: $(BISON_SRCS)"

$(PROJECT)-prebuild:: $(PROJECT)-bison-info $(BISON_SRCS)

$(PROJECT)-clean::
	$(ECHO) rm -f $(BISON_CPP_SRCS) $(BISON_CPP_HEADERS)

$(BISON_OUT)/%.c $(BISON_OUT)/%.h: $(BISON_BASE)/%.y
	$(INFO) "Running bison for $<..."
	$(ECHO) mkdir -p "$(BISON_OUT)"
	$(ECHO) $(BISON) --report=all --output="$@" -v "$<"


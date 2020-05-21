
# This invokes flex to create a lexer file.
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
# note that $($(PROJECT)_BASE) should be included to avoid
# conflicting with similar patterns from other projects.

FLEX=flex

# get flex sources
FLEX_SRCS=$(filter %.l,$($(PROJECT)_SRCS))

# these next two variables need to match what SRC_EXTENSIONS says
# (where mabu always prepends $($(PROJECT)_BASE) to input and generated sources)

# presumed base of all flex source files
FLEX_BASE=$($(PROJECT)_BASE)

# where we want to write the files
FLEX_OUT=$(FLEX_BASE)/generated

# deduce the output files, for the clean rule
FLEX_CPP_SRCS = $(patsubst $(FLEX_BASE)/%.l,$(FLEX_OUT)/%.c,$(FLEX_SRCS))
FLEX_CPP_HEADERS = $(patsubst $(FLEX_BASE)/%.l,$(FLEX_OUT)/%.h,$(FLEX_SRCS))

$(PROJECT)-flex-info:
	$(SPAM) "flex sources: $(FLEX_SRCS)"

$(PROJECT)-prebuild:: $(PROJECT)-flex-info $(FLEX_SRCS)

$(PROJECT)-clean::
	$(ECHO)	rm -f $(FLEX_CPP_SRCS) $(FLEX_CPP_HEADERS)

$(FLEX_OUT)/%.c $(FLEX_OUT)/%.h: $(FLEX_BASE)/%.l
	$(INFO) "Running flex for $<..."
	$(ECHO) mkdir -p "$(FLEX_OUT)"
	$(FLEX) -CeF --outfile="$@" -v "$<"


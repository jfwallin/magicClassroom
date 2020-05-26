#
# This provides postbuild rules, which happen each time a 
# project is built (whether or not anything changed).
#
# (The EXTENSIONS setting in a mabu project must mention this file.)
#
# All the rules based on "postbuild" need to use the double-colon form.
#

# $(INFO) may be used to echo unless "-q" is passed, and $(ECHO) used for verbose messages when "-v" is passed.

# This adds a postbuild step, which dumps the sizes of the "program" executable.
# The .mabu base name (with any spaces converted to '_') is used in the _BASE variable's name.
#
# (program_BASE may also be used, as in prebuilds)
#
postbuild:: $(program_OUTPUT_FILE)
	$(INFO) Postbuild step
ifneq ($(HOST), win64)
	size "$<" > "$<.txt"
else
	dumpbin -summary "$<" > "$<.txt"
endif

# An extension makefile can be applied to specific projects using
# $(PROJECT) to declare the postbuild rule or to reference project-specific
# variables.  mabu runs `$(PROJECT)-postbuild` after the C/C++ build steps
# and before any packaging steps.
#
# (If you need to target specific projects in rules, but must specify
# EXTENSIONS from on a different project, then the name of $(PROJECT) is the
# .mabu base filename with any spaces converted to underscores.)
#
$(PROJECT)-postbuild:: $($(PROJECT)_OUTPUT_FILE)
	@echo "Build complete for $(PROJECT): $($(PROJECT)_OUTPUT_FILE)"


#
# This provides prebuild rules, which run before a project is built
# (whether or not anything changed).
#
# (The EXTENSIONS setting in a mabu project must mention this file.)
#
# All the rules based on "prebuild" need to use the double-colon form.
#

# $(INFO) may be used to echo unless "-q" is passed, and $(ECHO) used for verbose messages when "-v" is passed.

# This runs at the top level before any other projects in the build tree.
prebuild::
	$(ECHO) "Running at prebuild..."

# Specific projects can have prebuild/postbuild steps, which run immediately
# before the project is built.  The rule name is the .mabu base filename with
# any spaces converted to underscores, followed by "-prebuild".
shared-prebuild:: 
	$(INFO) "Prebuilding for 'shared'"

# An extension makefile can be applied to specific projects using
# $(PROJECT) to declare the prebuild rule or to reference project-specific
# variables.  mabu runs `$(PROJECT)-prebuild` before the C/C++ build steps.
#
# (If you need to target specific projects in rules, but must specify
# EXTENSIONS from on a different project, then the name of $(PROJECT) is the
# .mabu base filename with any spaces converted to underscores.)
#
$(PROJECT)-prebuild:: $($(PROJECT)_BASE)/generated/gen_program_header.h

$($(PROJECT)_BASE)/generated/gen_program_header.h:
	$(INFO) "Prebuilding $@..."
	@mkdir -p $($(PROJECT)_BASE)/generated
	@echo "#define PROGRAM_MACRO 123" > "$@"




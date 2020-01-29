
_VALID=1

if [ -n "$BASH" ]; then
  [[ "${BASH_SOURCE[0]}" == "${0}" ]] && echo "This script should only be sourced, not executed!"

  _VDROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
else
  # Handle other bourne (sh) compatible shells. E.g. dash, posh, zsh
  # and others.
  # Note: This script does not support csh/tcsh and other types of shells.
  _VDROOT="$( cd "$( dirname "${0}" )" && pwd )"
fi

if [ -z "$MLSDK" ] || [ ! -d "$MLSDK/.metadata" ] ; then
    # MLSDK was either never set or doesn't match the expected directory.
	# We assume this script lives in VirtualDevice above MLSDK.
    source "$_VDROOT/../envsetup.sh"
    if [ $? != 0 ]; then
		unset _VALID
    fi
fi

function set_os {
	local name=`uname`
	case "$name" in
		Darwin)
			OS=osx; LIBVAR=DYLD_LIBRARY_PATH; _LIBVAL=$DYLD_LIBRARY_PATH ;;
		Linux)
			OS=linux64; LIBVAR=LD_LIBRARY_PATH; _LIBVAL=$LD_LIBRARY_PATH ;;
		MINGW32* | MINGW64* | MSYS*)
			OS=win64; LIBVAR=PATH; _LIBVAL=$PATH ;;
		*)
			echo '!!! Cannot determine HOST_OS from '$name
			unset _VALID ;;
	esac
}

set_os

if [ -z "$_VALID" ] ; then
	echo '!!! Could detect runtime environment.  Please verify the Lumin SDK layout is complete.'

else
	# locate sdk shim libraries instead of (and before) stubs
	export $LIBVAR="$_VDROOT/lib:$MLSDK/lib/$OS:$_LIBVAL"
	# macOS hides DYLD_LIBRARY_PATH for subprocesses, like mabu, so mirror this here
	# so mabu can fetch it before it's lost
	export ML_LIBRARY_PATH="$_VDROOT/lib:$MLSDK/lib/$OS:$_LIBVAL"

	# allow running from anywhere
	export PATH="$_VDROOT/bin:$PATH"

	export ML_VDS_DATA="$_VDROOT/data"
	echo "*** Environment now set up to run tools and MLSDK apps under Zero Iteration."
	
	# Hack to set exe bit on binaries
	# Remove once mabu can copy attributes during a package step
	chmod +x "$_VDROOT/bin/"*
fi

unset _VALID
unset _VDROOT

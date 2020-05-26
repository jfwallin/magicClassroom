# Note: This script should not be run, just sourced into an
# shell ("source ./envsetup.sh" or ". ./envsetup.sh").
# it has no effect when running it ("./envsetup.sh").

if [ -n "$BASH" ]; then
  [[ "${BASH_SOURCE[0]}" == "${0}" ]] && echo "This script should only be sourced, not executed!"

  _ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
  if [ -f "$_ROOT/internal/tools/mldb/mldb.bash" ]; then
    . "$_ROOT/internal/tools/mldb/mldb.bash"
  elif [ -f "$_ROOT/tools/mldb/mldb.bash" ]; then
    . "$_ROOT/tools/mldb/mldb.bash"
  fi
else
  # Handle other bourne (sh) compatible shells. E.g. dash, posh, zsh
  # and others.
  # Note: This script does not support csh/tcsh and other types of shells.
  _ROOT="$( cd "$( dirname "${0}" )" && pwd )"
fi


if [ ! -z "$MLSDK" ]; then
    # compare any existing MLSDK value
    if [ ! "$MLSDK" -ef "$_ROOT" ]; then
        echo "*** Replacing MLSDK=$MLSDK..."
    fi
fi

echo "*** Setting MLSDK=$_ROOT and updating PATH"
export MLSDK=$_ROOT

export PATH=$_ROOT:$_ROOT/internal/tools/mldb:$_ROOT/tools/mldb:$_ROOT/lumin/runtime/mxs/bin:$PATH


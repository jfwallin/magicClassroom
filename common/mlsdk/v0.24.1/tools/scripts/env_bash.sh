# Note: This script should not be run, just sourced into an
# shell ("source envsetup.sh" or ". ./env_bash.sh").
# it has no effect when running it ("./env_bash.sh").
#
# Helper script to source the default bash setup script as well as envsetup.sh
# 
# One use is to start a new terminal with bash initialized for the mlsdk
# Example usage:
# > gnome-terminal -e 'bash --init-file ~/MagicLeap/mlsdk/v0.10.0/tools/scripts/env_bash.sh -i'

# sourcing standard bash resource files
if [ -f /etc/profile ]; then
  . /etc/profile
elif [ -f ~/.bash_profile ]; then
  . ~/.bash_profile
elif [ -f ~/.bash_login ]; then
  . ~/.bash_login
elif [ -f ~/.profile ]; then
  . ~/.profile
fi

mydir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ -f "$mydir/../../VirtualDevice/mlvdsetup.sh" ]; then
  . "$mydir/../../VirtualDevice/mlvdsetup.sh"
elif [ -f "$mydir/../../envsetup.sh" ]; then
  . "$mydir/../../envsetup.sh"
else
  echo "*** Missing envsetup.sh and mlvdsetup.sh!"
fi


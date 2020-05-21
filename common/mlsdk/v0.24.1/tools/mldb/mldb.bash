# /* vim: set ai ts=4 ft=sh: */
#
# Copyright 2011, The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Modified to support mldb (derived from adb.bash)

# For zsh, autoload bashcompinit
if [ -n "$ZSH_VERSION" ]; then
    autoload -U +X compinit && compinit
    autoload -U +X bashcompinit && bashcompinit
fi

_mldb() {
    local binname=${1:-mldb}
    unset -v have
    type "$binname" &> /dev/null && have="yes"

    if [ "$have" != "yes" ]; then
        return
    fi

    local wait_prefix OPTIONS
    wait_prefix="wait-for-device"
    OPTIONS="-d -s"
    local where i cur serial
    COMPREPLY=()

    serial="${ANDROID_SERIAL:-none}"
    where=OPTIONS

    for ((i=1; i <= COMP_CWORD; i++)); do
        cur="${COMP_WORDS[i]}"
        case "${cur}" in
            -s)
                where=OPT_SERIAL
                ;;
            -*)
                where=OPTIONS
                ;;
            wait-for-*)
                where=OPTIONS
                wait_prefix=""
                OPTIONS=""
                ;;
            *)
                if [[ $where == OPT_SERIAL ]]; then
                    where=OPT_SERIAL_ARG
                    serial=${cur}
                elif [[ $where == OPT_SERIAL_ARG && ${cur} == ":" ]]; then
                    # The device is split into multiple entries for a TCP name with a port:
                    # "-s host:5555 xxx" -> is split into ["-s", "host", ":", "5555", "xxx"]
                    where=OPT_SERIAL_ARG_COLON
                    serial="${serial}${cur}"
                elif [[ $where == OPT_SERIAL_ARG_COLON ]]; then
                    where=COMMAND
                    serial="${serial}${cur}"
                else
                    where=COMMAND
                    break
                fi
                ;;
        esac
    done

    if [[ $where == COMMAND && $i -ge $COMP_CWORD ]]; then
        where=OPTIONS
    fi
    local gen_commands="bugreport connect devices disconnect forward get-serialno get-state help kill-server reboot reverse start-server tcpip usb version"
    local ml_commands_a_m="attach audiocapture audioplayer battery capture crashdump date df fsnotify getprop get-devpath get-name get-standby install keygen launch log"
    local ml_commands_n_z="notify packages perf provision ps reset-headpose rtsp-token server setprop shutdown terminate top uninstall"
    local oobe_commands="access-status lock unlock changepin"
    local sync_commands="cp ls mkdir mv pull push rm"
    local connection_commands="controller ifconfig mlma ping wifi"
    local cert_commands="tls-cert-add tls-cert-remove tls-cert-list"
    local public_commands="${gen_commands} ${ml_commands_a_m} ${ml_commands_n_z} ${oobe_commands} ${sync_commands} ${connection_commands} ${cert_commands}"
    COMMAND="${public_commands}"
    case $where in
        OPTIONS|OPT_SERIAL|OPT_PATH)
            COMPREPLY=( $(compgen -W "$OPTIONS $COMMAND $wait_prefix" -- "$cur") )
            ;;
        OPT_SERIAL_ARG)
            local devices=$(command mldb devices 2> /dev/null | grep -v "List of devices" | awk '{ print $1 }')
            COMPREPLY=( $(compgen -W "${devices}" -- "${cur}") )
            ;;
        COMMAND)
            if [[ $i -eq $COMP_CWORD ]]; then
                COMPREPLY=( $(compgen -W "$COMMAND" -- "$cur") )
            else
                i=$((i+1))
                case "${cur}" in
                    attach)
                        _mldb_cmd_attach "$serial" $i
                        ;;
                    launch)
                        _mldb_cmd_launch "$serial" $i
                        ;;
                    ls)
                        _mldb_cmd_ls "$serial" $i
                        ;;
                    mv)
                        _mldb_cmd_mv "$serial" $i
                        ;;
                    cp)
                        _mldb_cmd_cp "$serial" $i
                        ;;
                    install)
                        _mldb_cmd_install "$serial" $i
                        ;;
                    pull)
                        _mldb_cmd_pull "$serial" $i
                        ;;
                    push)
                        _mldb_cmd_push "$serial" $i
                        ;;
                    reboot)
                        if [[ $COMP_CWORD == $i ]]; then
                            args="bootloader recovery"
                            COMPREPLY=( $(compgen -W "${args}" -- "${COMP_WORDS[i]}") )
                        fi
                        ;;
                    rm)
                        _mldb_cmd_rm "$serial" $i
                        ;;
                    mkdir)
                        _mldb_cmd_mkdir "$serial" $i
                        ;;
                    controller)
                        _mldb_cmd_controller "$serial" $i
                        ;;
                    capture)
                        _mldb_cmd_capture "$serial" $i
                        ;;

                    terminate)
                        _mldb_cmd_terminate "$serial" $i
                        ;;
                    uninstall)
                        _mldb_cmd_uninstall "$serial" $i
                        ;;
                    crashdump)
                        _mldb_cmd_crashdump "$serial" $i
                        ;;
                    wifi)
                        _mldb_cmd_wifi "$serial" $i
                        ;;
                    audiocapture)
                        _mldb_cmd_audiocapture "$serial" $i
                        ;;
                    audioplayer)
                        _mldb_cmd_audioplayer "$serial" $i
                        ;;
                esac
            fi
            ;;
    esac

    return 0
}

_mldb_cmd_install() {
    local serial i cur where

    serial=$1
    i=$2

    cur="${COMP_WORDS[COMP_CWORD]}"
    COMPREPLY=( $(compgen -W "-u" -- "$cur") )

    _mldb_util_complete_local_file "${cur}" '!*.mpk'
}

_mldb_cmd_launch() {
    # launch [-f] [-d -p <host-port>] <pkg-name> [<comp-name>]
    _mldb_package_command "$1" "$2" "-f -d -p -w" "all"
}

_mldb_cmd_terminate() {
    # terminate [-f] <pkg-name> [<comp-name>]
    _mldb_package_command "$1" "$2" "-f" "running"
}

_mldb_cmd_attach() {
    # attach [-p <host-port>] <pkg-name> [<comp-name>]
    # TODO: this currently lists all running packages, could list just all debuggable, not yet debugged, running processes instead
    _mldb_package_command "$1" "$2" "-p" "running"
}

_mldb_cmd_uninstall() {
    # attach <pkg-name>
    local serial=$1
    if [[ $i == $COMP_CWORD ]]; then
        # TODO: only list user applications, not system ones.
        _mldb_util_list_packages "${serial}"
    fi
}

_mldb_package_command() {
    # one of the commands taking a <pkg-name> [<comp-name>] arguments:
    # launch [-f] [-d -p <host-port>] <pkg-name> [<comp-name>]
    # terminate [-f] <pkg-name> [<comp-name>]
    # attach [-p <host-port>] <pkg-name> [<comp-name>]
    local serial=$1
    local i=$2
    local options="$3"
    local filter_type="$4" # either "all" or "running"
    local where=PACKAGE package="" component=""

    for ((; i <= COMP_CWORD; i++)); do
        cur="${COMP_WORDS[i]}"
        case "${cur}" in
            -p)
                if [[ $i == $COMP_CWORD ]]; then
                    where=OPTIONS
                    break
                fi
                ((i++))
                if [[ $i == $COMP_CWORD ]]; then
                    return 0 # No completion when on the port number. Should we suggest a default one? 10000?
                fi
                ;;
            -*)
                ;;
            *)
                if [[ "${package}" == "" ]]; then
                    where=PACKAGE
                    package="${cur}"
                elif [[ "${component}" == "" ]]; then
                    where=COMPONENT
                    component="${cur}"
                else
                    where=OPTIONS
                    break
                fi
                ;;
        esac
    done

    local cur="${COMP_WORDS[COMP_CWORD]}"
    if [[ $where == PACKAGE ]]; then
        COMPREPLY=( $(compgen -W "${options}" -- "${cur}") )
        if [[ $filter_type == "all" ]]; then
             _mldb_util_list_packages "${serial}"
        elif [[ $filter_type == "running" ]]; then
             _mldb_util_list_running "${serial}" ""
        fi
    elif [[ $where == COMPONENT ]]; then
        COMPREPLY=( $(compgen -W "${options}" -- "${cur}") )
        if [[ $filter_type == "all" ]]; then
            _mldb_util_list_components "${serial}" "${package}"
        elif [[ $filter_type == "running" ]]; then
            _mldb_util_list_running "${serial}" "${package}"
        fi
    elif [[ $where == OPTIONS ]]; then
        COMPREPLY=( $(compgen -W "${options}" -- "${cur}") )
    fi
}

# Helper to handle -p option in push/pull/ls and other commands. And because
# now exteral devs can alternatively specify -D instead of -p <package>, in
# order to acceess the documents folder, this function also checks for that
# argument and if found, sets g_docs_relative to 1. Obviously, in that case,
# no package name completion is done.
#
# Searches through the options and if it finds a -p option, then it handles
# the completion of the -p option and its argument. The global variable
# g_package is set with the package name as given with -p, or to an empty
# string if no -p is found.
#
# On return, the global g_file_arg_index is set to -1 if the cursor is on
# a switch or on the package name after -p. Otherwise it's a zero-based index
# indicating which filepath argument the cursor is on, given that all such
# other arguments in the cmdline are presumably filepaths.
# So, e.g.,
#      mldb push -p come.acme.myapp /tmp/somefile /data/local/tmp
# g_file_arg_index is 0 when the cursor is on /tmp/somefile and 1 when
# on /data/local/tmp
#
# @param serial - either "none" or the serial id of the device to use
# @param i - index into COMP_WORDS of the remaining arguments, to skip already parsed parts of the command line.
# @param options - a string containing all the supported options of the command, space separated.
#
_handle_package_opt() {
    local serial=$1
    local i=$2
    local options="$3"
    local cur="${COMP_WORDS[COMP_CWORD]}"
    g_file_arg_index=-1 # global, set to argument number for non option arguments
    g_package="" # global, part of the return value of this function.
    g_docs_relative=0

    # The option -p is required. Hence suggest it as first option.
    if [[ $COMP_CWORD == "$i" && "$cur" == "" ]]; then
        COMPREPLY=("-p")
        COMPREPLY+=("-D")
        return
    fi

    # Add the given options to the possible completions
    COMPREPLY=( ${COMPREPLY[@]:-} $(compgen -W "$options" -- "$cur") )
    for ((; i <= COMP_CWORD; i++)); do
        cur="${COMP_WORDS[i]}"
        case "${cur}" in
            -p)
                if [[ $COMP_CWORD == "$i" ]]; then
                    g_file_arg_index=-1
                    return
                fi
                ((i++))
                g_package="${COMP_WORDS[i]}"
                if [[ $COMP_CWORD == "$i" ]]; then
                    COMPREPLY=() # Clear out options as possible completions.
                    _mldb_util_list_packages "${serial}"
                    g_file_arg_index=-1
                    return
                fi
                ;;
            -D)
                if [[ $COMP_CWORD == "$i" ]]; then
                    g_file_arg_index=-1
                    return
                fi
                g_docs_relative=1
                ;;
            -*)
                if [[ $COMP_CWORD == "$i" ]]; then
                    g_file_arg_index=-1
                    return
                fi
                ;;
            *)
                ((g_file_arg_index++))
                if [[ $COMP_CWORD == "$i" ]]; then
                    # The only place where we do not reset g_file_arg_index to -1
                    return
                fi
                ;;
        esac
    done
    #Note: Should not get here as we return for $COMP_CWORD == "$i" in every case entry above.
    g_file_arg_index=-1
}

_mldb_filesystem_cmd() {
    local serial=$1
    local i=$2
    local valid_opts="$3"
    _handle_package_opt $serial $i "$valid_opts"

    # If the cursor is on a filepath argument, try to complete it
    if [[ $g_file_arg_index -ge 0 ]]; then
        local path_prefix="${COMP_WORDS[COMP_CWORD]}"
        if [[ -z $path_prefix ]]; then
            path_prefix="/"
        fi
        _mldb_util_list_files "$serial" "$g_docs_relative" "$g_package" "$path_prefix"
    fi
}

_mldb_cmd_ls() {
    _mldb_filesystem_cmd $1 $2 "-p -D -l -d -t -S -a -A -m -R -C"
}

_mldb_cmd_mv() {
    _mldb_filesystem_cmd $1 $2 "-p -D -n -f"
}

_mldb_cmd_cp() {
    _mldb_filesystem_cmd $1 $2 "-p -D -n -f -r"
}

_mldb_cmd_rm() {
    _mldb_filesystem_cmd $1 $2 "-p -D -f -r"
}

_mldb_cmd_mkdir() {
    _mldb_filesystem_cmd $1 $2 "-p -D"
}

_mldb_cmd_controller() {
    local serial=$1
    local i=$2

    local cur="${COMP_WORDS[COMP_CWORD]}"
    if [[ $i -eq $COMP_CWORD ]]; then
        COMPREPLY=( $(compgen -W "scan connect forget status info" -- "$cur") )
    else
        COMPREPLY=( ${COMPREPLY[@]:-} $(compgen -W "$options" -- "$cur") )
    fi
}

_mldb_cmd_capture() {
    local serial=$1
    local i=$2

    local valid_opts="-r -q -b"   # -f -t -w added only if video capture
    local param_opts="-q -b -f -t"
    local cur="${COMP_WORDS[COMP_CWORD]}"
    local file_arg_index=-1 # set to argument number for non option arguments

    if [[ $i -eq $COMP_CWORD ]]; then
        COMPREPLY=( $(compgen -W "image video" -- "$cur") )
    else
        # Add the given options to the possible completions
        for ((; i <= COMP_CWORD; i++)); do
            cur="${COMP_WORDS[i]}"

            case "${cur}" in
                -*)
                    if [[ $COMP_CWORD == "$i" ]]; then
                        file_arg_index=-1
                        break
                    fi
                    ;;
                *)
                    local prior=${COMP_WORDS[(i-1)]}
                    if [[ " ${param_opts[@]} " =~ " ${prior} " ]]; then
                        # don't try to parse filenames following an option with a value parameter
                        if [[ $COMP_CWORD == "$i" ]]; then
                            case "${prior}" in
                                -q)
                                    COMPREPLY=( $(compgen -W "1080p 720p" -- "$cur") )
                                    ;;
                                -b)
                                    COMPREPLY=( $(compgen -W "alpha additive hybrid" -- "$cur") )
                                    ;;
                                *)
                                    COMPREPLY=()
                                    ;;
                            esac
                            return 0
                        fi
                    else
                        if [[ $cur == "video" ]]; then
                            valid_opts=${valid_opts[@]:-}" -f -t -w"
                        fi
                        ((file_arg_index++))
                    fi
                    ;;
            esac
        done
        if [[ $file_arg_index -eq 1 ]]; then
            _mldb_util_complete_local_file "${cur}"
        fi
        COMPREPLY=( ${COMPREPLY[@]:-} $(compgen -W "$valid_opts" -- "$cur") )
    fi
}

_mldb_cmd_push() {
    local serial=$1
    local i=$2
    valid_opts="-p -D -v"
    _handle_package_opt $serial $i "$valid_opts"

    local cur="${COMP_WORDS[COMP_CWORD]}"
    if [[ $g_file_arg_index -eq 0 ]]; then
        _mldb_util_complete_local_file "${cur}"
    elif [[ $g_file_arg_index -eq 1 ]]; then
        _mldb_filesystem_cmd $serial $i $valid_opts
    fi
}

_mldb_cmd_pull() {
    local serial=$1
    local i=$2
    valid_opts="-p -D -v -a"
    _handle_package_opt $serial $i "$valid_opts"

    local cur="${COMP_WORDS[COMP_CWORD]}"
    if [[ $g_file_arg_index -eq 0 ]]; then
        _mldb_filesystem_cmd $serial $i $valid_opts
    elif [[ $g_file_arg_index -eq 1 ]]; then
        _mldb_util_complete_local_file "${cur}"
    fi
}

_mldb_cmd_audiocapture() {
    local serial=$1
    local i=$2
    local valid_opts="-a -w -v -d"
    local param_opts="-d"
    _mldb_util_audio_cmd_helper "$serial" "$i" "$valid_opts" "$param_opts"
}

_mldb_cmd_audioplayer() {
    local serial=$1
    local i=$2
    local valid_opts="-s -x -y -z"
    local param_opts="-x -y -z"
    _mldb_util_audio_cmd_helper "$serial" "$i" "$valid_opts" "$param_opts"
}

_mldb_util_audio_cmd_helper() {
    local serial=$1
    local i=$2
    local valid_opts="$3"
    local param_opts="$4" # options which require a value parameter (e.g. -d 10)

    local cur="${COMP_WORDS[COMP_CWORD]}"
    local file_arg_index=-1 # set to argument number for non option arguments

    # Add the given options to the possible completions
    for ((; i <= COMP_CWORD; i++)); do
        cur="${COMP_WORDS[i]}"

        case "${cur}" in
            -*)
                if [[ $COMP_CWORD == "$i" ]]; then
                    file_arg_index=-1
                    break
                fi
                ;;
            *)
                if [[ " ${param_opts[@]} " =~ " ${COMP_WORDS[(i-1)]} " ]]; then
                    # don't try to parse filenames following an option with a value parameter
                    if [[ $COMP_CWORD == "$i" ]]; then
                        COMPREPLY=()
                        return 0
                    fi
                else
                    ((file_arg_index++))
                fi
                ;;
        esac
    done
    COMPREPLY=( ${COMPREPLY[@]:-} $(compgen -W "$valid_opts" -- "$cur") )

    cur="${COMP_WORDS[COMP_CWORD]}"
    if [[ $file_arg_index -eq 0 ]]; then
        local path_prefix="${COMP_WORDS[COMP_CWORD]}"
        # limit the search relative to the /C1/ folder
        if [[ ${path_prefix:0:4} != "/C1/" ]]; then
            path_prefix="/C1/${path_prefix}"
        fi

        # If the cursor is on a filepath argument, try to complete it
        _mldb_util_list_files "$serial" 1 "$g_package" "$path_prefix"

        # strip off the leading /C1/ from the filename
        for (( i=0; i<${#COMPREPLY[@]}; i++ ));
        do
            if [[ ${COMPREPLY[$i]:0:4} == "/C1/" ]]; then
                COMPREPLY[$i]=${COMPREPLY[$i]:4}
            fi
        done
    fi
}

_mldb_cmd_crashdump() {
    local serial=$1
    local i=$2
    # crashdump list
    # crashdump get (all | <pid> | <name>)
    local cmd="${COMP_WORDS[i]}"
    local cur="${COMP_WORDS[COMP_CWORD]}"
    if [[ $i -eq $COMP_CWORD ]]; then
        COMPREPLY=( $(compgen -W "list get" -- "$cur") )
    elif [[ $((i+1)) -eq $COMP_CWORD && "${COMP_WORDS[i]}" == "get" ]]; then
        # for now only suggesting all and not the pids or names of existing crashdumps
        COMPREPLY=( $(compgen -W "all" -- "$cur") )
    fi
}

_mldb_cmd_wifi() {
    local serial=$1
    local i=$2
    # For now auto-completing the main command only
    local cmd="${COMP_WORDS[i]}"
    local cur="${COMP_WORDS[COMP_CWORD]}"
    if [[ $i -eq $COMP_CWORD ]]; then
        COMPREPLY=( $(compgen -W "on off scan status configure forget list join getband setband" -- "$cur") )
    fi
}

_mldb_util_list_files() {
    local serial=$1
    local  dir IFS=$'\n'
    local -a toks
    local -a args
    local -a package_args
    local -a context_arg

    # Either '0' or '1'. Set to '1' when -D is specified, in which case [package]
    # is ignored
    local is_docs_relative="$2"

    # Name of a package specified with -p
    local package="$3"

    # Any starting portion of the filepath already specified in the cmdline. We'll
    # auto-complete based on this
    file="$4"

    if [ "$serial" != "none" ]; then
        args=(-s $serial)
    fi

    if [ "$is_docs_relative" == "1" ]; then
        context_arg=(-D)
    elif [ "$package" != "" ]; then
        context_arg=(-p $package)
    else
        context_arg=()
    fi

    toks=( ${toks[@]-} $(
        # -m: to append / for directories
        command mldb ${args[@]} ls ${context_arg[@]} -dm ${file}"*" '2>/dev/null' 2> /dev/null | tr -d '\r'
    ))

    # Since we're probably doing file completion here, don't add a space after.
    if [[ $(type -t compopt 2> /dev/null) = "builtin" ]]; then
        compopt -o nospace
    fi

    COMPREPLY=( ${COMPREPLY[@]:-} "${toks[@]}" )
}

_mldb_util_list_packages() {
    local serial="$1"

    local -a args=()
    if [ "$serial" != "none" ]; then
        args=(-s $serial)
    fi

    # cut -d ' ' -f 1: show the first entry (just the package name)
    # tail -n +2 : drop the title line
    # tr -d '\r' : delete cr
    # tr '\r\n' '  ' : use space instead of CW/Newlines
    local all_packages=$(command mldb ${args[@]} packages 2> /dev/null | cut -d ' ' -f 1 | tail -n +2 | tr '\r\n' '  ')
    # filter out all packages, so only matching ones remain
    local cur="${COMP_WORDS[COMP_CWORD]}"
    local -a matching_packages=( $(compgen -W "${all_packages}" -- "${cur}") )

    COMPREPLY=( ${COMPREPLY[@]:-} "${matching_packages[@]}" )
}

_mldb_util_list_components() {
    local serial python_code cur args
    serial="$1"
    package="$2"
    cur="${COMP_WORDS[COMP_CWORD]}"

    if [ "$serial" != "none" ]; then
        args="'-s', '"
        args="${args}${serial}"
        args="${args}', "
    fi

    # Using python to properly extract the component names
    # from the JSON output retrieved via mldb packages -j

    read -r -d '' python_code <<- EOM
import subprocess, sys, json

out = subprocess.Popen(['mldb', ${args} 'packages', '-j'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
stdout, stderr = out.communicate()
try:
  lst = json.loads(stdout)
except:
  # Ignoring failures for auto completion behavior
  exit(0)

for pack in lst:
  if pack['package'] == '$package':
    components=pack['components']
    for component in components:
      sys.stdout.write(component['name'] + ' ')
EOM
    all_components=$(command echo "$python_code" | python)
    matching_components=( $(compgen -W "${all_components}" -- "${cur}") )
    COMPREPLY=( ${COMPREPLY[@]:-} "${matching_components[@]}" )
}

_mldb_util_list_running() {
    local serial python_code cur args
    serial="$1"
    package="$2"
    cur="${COMP_WORDS[COMP_CWORD]}"

    if [ "$serial" != "none" ]; then
        args="'-s', '"
        args="${args}${serial}"
        args="${args}', "
    fi

    # Using python to properly extract the running packages/components
    # from the JSON output retrieved via mldb ps -j

    read -r -d '' python_code <<- EOM
import subprocess, sys, json

out = subprocess.Popen(['mldb', ${args} 'ps', '-j'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
stdout, stderr = out.communicate()
try:
  lst = json.loads(stdout)
except:
  # Ignoring failures for auto completion behavior
  exit(0)

for pack in lst:
  package_name = pack['package']
  if '$package' == "":
    # list all running packages
    sys.stdout.write(package_name + ' ')
  else:
    # list all running components of a specific package
    if package_name == '$package':
      component=pack['component']
      sys.stdout.write(component + ' ')
EOM
    result_list=$(command echo "$python_code" | python)
    matching_results=( $(compgen -W "${result_list}" -- "${cur}") )
    COMPREPLY=( ${COMPREPLY[@]:-} "${matching_results[@]}" )
}


_mldb_util_complete_local_file()
{
    local file xspec i j IFS=$'\n'
    local -a dirs files

    file=$1
    xspec=$2

    # Since we're probably doing file completion here, don't add a space after.
    if [[ $(type -t compopt 2> /dev/null) = "builtin" ]]; then
        compopt -o plusdirs
        if [[ "${xspec}" == "" ]]; then
            COMPREPLY=( ${COMPREPLY[@]:-} $(compgen -f -- "${cur}") )
        else
            compopt +o filenames
            COMPREPLY=( ${COMPREPLY[@]:-} $(compgen -f -X "${xspec}" -- "${cur}") )
        fi
    else
        # Work-around for shells with no compopt

        dirs=( $(compgen -d -- "${cur}" ) )

        if [[ "${xspec}" == "" ]]; then
            files=( ${COMPREPLY[@]:-} $(compgen -f -- "${cur}") )
        else
            files=( ${COMPREPLY[@]:-} $(compgen -f -X "${xspec}" -- "${cur}") )
        fi

        COMPREPLY=( $(
            for i in "${files[@]}"; do
                local skip=
                for j in "${dirs[@]}"; do
                    if [[ $i == $j ]]; then
                        skip=1
                        break
                    fi
                done
                [[ -n $skip ]] || printf "%s\n" "$i"
            done
        ))

        COMPREPLY=( ${COMPREPLY[@]:-} $(
            for i in "${dirs[@]}"; do
                printf "%s/\n" "$i"
            done
        ))
    fi
}


if [[ $(type -t compopt 2> /dev/null) = "builtin" ]]; then
    complete -F _mldb mldb
else
    complete -o nospace -F _mldb mldb
fi

#!/bin/bash
#
#  Copyright (C) 2012 bjarneh
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# -----------------------------------------------------------------------
#
#  Bash command completion file oc
# 
#  Add this file somewhere where it gets sourced.
#  If you have sudo power it can be dropped into
#  /etc/bash_completion.d/. If not it can be sourced
#  by one of your startup scripts (.bashrc .profile ...)
#

_oc(){

    local cur prev opts oc_long_opts oc_short_opts oc_short_explain oc_special
    # long options
    oc_long_opts="--help --version --dryrun --print --single --quiet --verbose --clean --static --sort --list --test --match --output --testbin --src --backend --jobs"
    # short options + explain
    oc_short_explain="-h[--help] -v[--version] -d[--dryrun] -p[--print] -1[--single] -j[--jobs] -q[--quiet] -V[--verbose] -c[--clean] -S[--static] -z[--sort] -i[--list] -m[--match] -o[--output] -t[--test] -T[--testbin] -s[--src] -b[--backend]"
    # short options
    oc_short_opts="-h -v[--version] -d -p -j -1 -q -V -c -S -z -i -m -o -t -T -s -b"

    oc_special="clean test help print"

    COMPREPLY=()

    prev="${COMP_WORDS[COMP_CWORD-1]}"
    cur="${COMP_WORDS[COMP_CWORD]}"

    if [[ "${cur}" == --* ]]; then
        COMPREPLY=( $(compgen -W "${oc_long_opts}" -- "${cur}") )
        return 0
    fi

    if [[ "${cur}" == -* ]]; then
        COMPREPLY=( $(compgen -W "${oc_short_opts}" -- "${cur}") )
        if [ "${#COMPREPLY[@]}" -gt 1 ]; then
            COMPREPLY=( $(compgen -W "${oc_short_explain}" -- "${cur}") )
        fi
        return 0
    fi

    case "${cur}" in
        c* | t* | h* | f* | s* | p* | d* | l*)
          COMPREPLY=( $(compgen -W "${oc_special}" -- "${cur}") )
          if [ "${#COMPREPLY[@]}" -gt 1 ]; then
              return 0
          fi
        ;;
    esac

    if [[ "${prev}" == -* ]]; then
        case "${prev}" in
            '-b' |'-b=' | '-backend' |'--backend' |'-backend=' |'--backend=')
                COMPREPLY=( $(compgen -W "clang gcc" -- "${cur}") )
                return 0
                ;;
        esac
    fi
}

## directories only -d was a bit to strict
complete -o default -F _oc oc

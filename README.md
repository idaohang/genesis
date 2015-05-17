# Genesis
A Multi-station GNSS Receiver

## What is Genesis?

Genesis is a simple coordinating program for running a multi-station GNSS receiver. It makes use of GNSS-SDR as a software-defined GNSS receiver, and RTKLIB for integer ambiguity resolution.

## How do I use it?

First, you'll need to build GNSS-SDR. Fetch it from https://github.com/gnss-sdr/gnss-sdr.

Next, you'll need a couple of receivers. These are typically Raspberry Pis (although alternatives such as the Orange Pi might also work.) Load them up with the `egnss` software from https://github.com/anthony-arnold/egnss.

Finally, you can run Genesis.

## Running Genesis

Firstly, do a typical cmake build:

    mkdir build && cd build && cmake .. && make

Start up Genesis with the `genesis` command. If you just built it, it will be in the `src` directory. Here's an example, illustrating the command line arguments available. They should be pretty self explanatory.

    src/genesis

The following arguments are available:

      Flags from /home/anthony/Development/genesis/src/main.cpp:
    -cal_config_file (The front-end-cal configuration file to use.)
      type: string default: "/usr/local/share/gnss-sdr/conf/front-end-cal.conf"
    -config_file (The GNSS-SDR configuration file to use.) type: string
      default: "/usr/local/share/gnss-sdr/conf/gnss-sdr.conf"
    -front_end_cal (The front-end-cal executable) type: string
      default: "/usr/local/bin/front-end-cal"
    -gnss_sdr (The gnss-sdr executable) type: string
      default: "/usr/local/bin/gnss-sdr"
    -listen_address (The address to listen to pings from (can be multicast).)
      type: string default: "0.0.0.0"
    -socket_file (The domain socket to open) type: string
      default: "/var/run/genesis.socket"
    -verbose (Verbose output) type: bool default: false
    -very_verbose (Very verbose output) type: bool default: true



  Flags from src/gflags.cc:
    -flagfile (load flags from file) type: string default: ""
    -fromenv (set flags from the environment [use 'export FLAGS_flag1=value'])
      type: string default: ""
    -tryfromenv (set flags from the environment if present) type: string
      default: ""
    -undefok (comma-separated list of flag names that it is okay to specify on
      the command line even if the program does not define a flag with that
      name.  IMPORTANT: flags in this list that have arguments MUST use the
      flag=value format) type: string default: ""

  Flags from src/gflags_completions.cc:
    -tab_completion_columns (Number of columns to use in output for tab
      completion) type: int32 default: 80
    -tab_completion_word (If non-empty, HandleCommandLineCompletions() will
      hijack the process and attempt to do bash-style command line flag
      completion on this value.) type: string default: ""

  Flags from src/gflags_reporting.cc:
    -help (show help on all flags [tip: all flags can have two dashes])
      type: bool default: false currently: true
    -helpfull (show help on all flags -- same as -help) type: bool
      default: false
    -helpmatch (show help on modules whose name contains the specified substr)
      type: string default: ""
    -helpon (show help on the modules named by this flag value) type: string
      default: ""
    -helppackage (show help on all modules in the main package) type: bool
      default: false
    -helpshort (show help on only the main module for this program) type: bool
      default: false
    -helpxml (produce an xml version of help) type: bool default: false
    -version (show version and build info and exit) type: bool default: false

## Connecting Stations

Now that you have Genesis running, and you've built a couple of stations (your Raspberry Pis), you can connect them up. Simply turn the stations on; as long as you've configured the networking on them correctly, they should automatically be detected by Genesis, which will start reading from them.


## Contact

Anthony Arnold

University of Queensland

`anthony.arnold@uqconnect.edu.au`

----
This file is part of Genesis.

    Genesis is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genesis is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Genesis.  If not, see <http://www.gnu.org/licenses/>.
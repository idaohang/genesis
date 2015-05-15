# Genesis
A Multi-station GNSS Receiver

## What is Genesis?

Genesis is a simple coordinating program for running a multi-station GNSS receiver. It makes use of GNSS-SDR as a software-define GNSS receiver, and RTKLIB for integer ambiguity resolution.

## How do I use it?

First, you'll need to build GNSS-SDR. Fetch it from https://github.com/gnss-sdr/gnss-sdr.

Next, you'll need a couple of receivers. These are typically Raspberry Pis (although alternatives such as the Orange Pi might also work.) Load them up with the `egnss` software from https://github.com/anthony-arnold/egnss.

Finally, you can run Genesis.

## Running Genesis

Firstly, do a typical cmake build:

    mkdir build && cd build && cmake .. && make

Start up Genesis with the `genesis` command. If you just built it, it will be in the `src` directory. Here's an example, illustrating the command line arguments available. They should be pretty self explanatory.

    src/genesis

The following arguments (with defaults) are available:

 - `--gnss_sdr` (/usr/local/bin/gnss-sdr)
 - `--front_end_cal` (/usr/local/bin/front-end-cal)
 - `--config_file` (/usr/local/share/gnss-sdr/conf/gnss-sdr.conf)
 - `--cal_config_file` (/usr/local/share/gnss-sdr/conf/front-end-cal.conf)

## Connecting Stations

Now that you have Genesis running, and you've built a couple of stations (your Raspberry Pis), you can connect them up. Simply turn the stations on; as long as you've configured the networking on them correctly, they should automatically be detected by Genesis, which will start reading from them.

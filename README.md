This is an attempt to interface an Arduino (and ultimately a PC) with an
IBM 6715 Typewriter.

There is absolutely zero information online about this typewriter's
serial interface, except one brief bit of code in German in an old
CPC magazone. Balj has done some translation of said code ([notes here](http://flatpack.microwavepizza.co.uk/gabriele9009-cpc-driver.txt))
and between us we have managed to get something working - and this is it.

The Arduino uses pins 0/1 for the TX/RX, and pin 2 for the byte acknowlege signal
from the typewriter, and pin 3 for the "I want to send" signal to the typewriter.

(wiring diagram to follow)

----

Protocol
========

This is what we have managed to glean and gather about the protocol this typewriter talks.

It's based around a two-byte-per-packet protocol, and for some commands it's a nibble for
the command and 12 bits for the value associated with that command. The communication
is at 4800 baud.

The packets can be grouped into two halves separated by the high bit of the first byte being
set or not.

If the high bit is not set you are requesting a character to be typed. The first byte is the
character number, the second how hard to strike the character against the paper.  If the high
bit of the second byte is set the head will auto-advance to the next character.

If the high bit of the first byte is set then it's a system control packet.  These commands
use the first nibble to define the command then the following 12 bits as a single value.

* 0x8nnn - Set the character pitch  as nnn/120" 
* 0xAn00 - System configuration - see below.
* 0xCnnn - Move the carriage forward by nnn/120"
* 0xDnnn - Advance the paper by nnn/96"
* 0xEnnn - Move the carriage backwards by nnn/120"
* 0xFnnn - Retract the paper by nnn/96"


The system configuration commands An00 are a bit of a mystery.  There is an "online" command 
that initializes the typewriter into printer mode, which is 0xA100 0xA400 0xA200 0x821F, and 0xA000
switches it offline again (never to return without a power cycle it seems). The 0x821F seems to
be a bit of an anomaly in the formation of the commands, so we're a little puzzled by that one.


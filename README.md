This is an attempt to interface an Arduino (and ultimately a PC) with an
IBM 6715 Typewriter.

There is absolutely zero information online about this typewriter's
serial interface, except one brief bit of code in German in an old
CPC magazone. Balj has done some translation of said code ([notes here](http://flatpack.microwavepizza.co.uk/gabriele9009-cpc-driver.txt))
and between us we have managed to get something working - and this is it.

The Arduino uses pins 0/1 for the TX/RX, and pin 2 for the byte acknowlege signal
from the typewriter, and pin 3 for the "I want to send" signal to the typewriter.

(wiring diagram to follow)

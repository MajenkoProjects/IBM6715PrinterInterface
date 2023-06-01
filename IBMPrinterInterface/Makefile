
# This is what the board identifies as for rebooting
PORT=/dev/ttyACM0

# What the board is
BOARD=arduino:avr:leonardo

# Targets:
#
#     make          
#         Compile the ino
#
#     make install
#         Compile the ino and upload it to the board
#
#     make watch
#         Wait for changes to the file and compile automatically. Requires
#         `entr` to operate (sudo apt install entr)
#
#     make clean
#         Delete the build folder

SKETCH=$(shell basename $$(pwd))
INO=${SKETCH}.ino
SUBDIR=$(subst :,.,${BOARD})
BIN=bin/${SKETCH}.ino.hex

${BIN}: ${INO} 
	@echo "Sketch is" ${INO}
	@arduino-cli compile -b ${BOARD} --output-dir bin --build-path build ${INO}

install: ${BIN}
	@arduino-cli upload -b ${BOARD} --input-dir bin --port ${PORT}

clean:
	rm -rf build bin

watch:
	echo ${INO} | entr -c -s 'make'


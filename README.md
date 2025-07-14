# Gomoku-AI

Can an ATmega328P microcontroller run a Gomoku bot?

Gomoku is a traditional board game similar to Tic-Tac-Toe where players take turns placing stones
until somebody lines up five in a row.

Although not a complicated game, I wanted to see how well an ATmega328P could "think", and if it had the
computational power to be a formidable opponent. The current algorithm is very primitive, but I'm interested to see how
far I can improve it over time. So far, I'm quite happy with the performance of the ATmega328P. The only issue I had was
the amount of DRAM, restricting the board size down to 10x10, even though a traditional game is usually played with
15x15.

The C++ code is a simple recreation of the game Gomoku, and the ino file is to be uploaded to the arduino. The arduino
must also be connected to the game computer through a serial connection (the onboard USB-B is great). The game does not
implement any of the thinking. All bot moves are calculated purely on the microcontroller.

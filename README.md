# cnum

MNIST number recognition using pure ANSI C.

## Perceptron

Using a single-layer perceptron network with 10 neurons with inputs ranging from 0 to 1,
inclusive, I am able to obtain a 79% accuracy rate.

Problem numbers include #11 and #65 of t10k which are incorrectly recognized as 8 and #24 of t10k.

Run this just by doing `./cnum`. A pre-trained network is provided already and you can use it by answering 'Y' to the answer.

When compiled with `-O3`, training time is around 4 seconds on my Intel(R) Xeon(R) CPU E5-2640 v2 @ 2.00GHz
for the 60k set, but I train the network on the 60k set twice, so it takes around 8 seconds total.
Under valgrind and with `-O0`, the training time is around 142 seconds.

## Convolutional

Planned; NYI.

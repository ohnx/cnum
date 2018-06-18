# cnum

MNIST number recognition using pure ANSI C.

Credits to https://mmlind.github.io/Simple_1-Layer_Neural_Network_for_MNIST_Handwriting_Recognition/
for inspiring this project and http://yann.lecun.com/exdb/mnist/ for creating the MNIST database.

## Perceptron

Using a single-layer perceptron network with 10 neurons with inputs ranging from 0 to 1,
inclusive, I am able to obtain an 80% accuracy rate after the second round of training.

Problem numbers include #11 and #65 of t10k which are incorrectly recognized as 8 and #24 of t10k.

Run this just by doing `./cnum`. A pre-trained network is provided already and you can use it by answering 'Y' to the answer.

When compiled with `-O3`, training time is around 4 seconds on my Intel(R) Xeon(R) CPU E5-2640 v2 @ 2.00GHz
for the 60k set, but I train the network on the 60k set twice, so it takes around 8 seconds total.
Under valgrind and with `-O0`, the training time is around 142 seconds.

## Convolutional

Planned; NYI.

## Roadmap

1. Optimize matrix multiplication using BLAS and better memory management
    * See code comments for info on how to represent the network as a matrix
    * Target is < 1s train time exclusive of disk read
2. Integrate with tigr
    * Enable a user to draw their own numbers and have them recognized
3. Make the code more extensible
    * The idea is to not force a 784 vector
    * Only use two different kinds of vectors, double and integer
    * Will probably sacrifice some speed for this
4. Add support for multi-layer perceptron network
    * Logical progressioin after previous step
    * https://mmlind.github.io/Simple_3-Layer_Neural_Network_for_MNIST_Handwriting_Recognition/ may help
5. Implement convolutional network
    * Convolutional networks are where it's at!
    * https://mmlind.github.io/Deep_Neural_Network_for_MNIST_Handwriting_Recognition/ will probably help

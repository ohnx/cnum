# cnum

MNIST number recognition using ANSI C.

Credits to [@mmlind](https://mmlind.github.io/Simple_1-Layer_Neural_Network_for_MNIST_Handwriting_Recognition/)
for inspiring this project and [these people](http://yann.lecun.com/exdb/mnist/) for creating the MNIST database.
Also, thanks to Apple for maintaining some [incredibly nice-to-read BLAS documentation](https://developer.apple.com/documentation/accelerate/blas?language=objc).

## Perceptron network

Aside from `build-essential`, compilation requires BLAS.
Run `sudo apt-get install libopenblas-dev` on Debian systems to get it.

Afterwards, `make` will generate a nice little output of `cnum`.
Run this just by doing `./cnum`. A pre-trained network is provided already and you can use it by answering 'Y' to the question.

### Accuracy

Using a single-layer perceptron network with 10 neurons with inputs ranging from 0 to 1,
inclusive, I am able to obtain an accuracy rate of 85% after the fourth round of training.

Problem numbers include #11, #24, and #943 (all from t10k).

### Speed

When compiled with `-O3`, training time is around 0.8 seconds on my `Intel(R) Xeon(R) CPU E5-2640 v2 @ 2.00GHz`
for the 60k set, but I train the network on the 60k set twice, so it takes under 2 seconds total.
With debug mode (`-O0 -g -pg`), the training time is around 1 second each and 2 seconds total.
Thanks, BLAS :)

## Convolutional

Planned; NYI.

## Roadmap

1. ~~Optimize matrix multiplication using BLAS and better memory management~~ **Done on June 18, 2018!**
    * ~~Target is < 1s train time~~ Train time is ~0.7s when optimized
2. Integrate with tigr
    * Enable a user to draw their own numbers and have them recognized
4. Add support for multi-layer perceptron network
    * Logical progressioin after previous step
    * https://mmlind.github.io/Simple_3-Layer_Neural_Network_for_MNIST_Handwriting_Recognition/ may help
5. Implement convolutional network
    * Convolutional networks are where it's at!
    * https://mmlind.github.io/Deep_Neural_Network_for_MNIST_Handwriting_Recognition/ will probably help

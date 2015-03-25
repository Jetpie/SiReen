SiReen
=========

Similar Image REcommendation ENgine


#Introduction

The code is a implementation of similar image retrieval engine. The implementation is mainly in C++ and Python. The following functions are included:

* Dense SIFT feature extraction
* Local-constrained Linear Coding based on Dense SIFT.
* Fast high dimension feature retrieval by KD-Tree using Best-Bin-First algorithm and optimized comparison method

##References:
Jinjun Wang; Jianchao Yang; Kai Yu; Fengjun Lv; Huang, T.; Yihong Gong, "Locality-constrained Linear Coding for image classification, " Computer Vision and Pattern Recognition (CVPR), 2010 IEEE Conference on , vol., no., pp.3360,3367, 13-18 June 2010

Lowe, D. Distinctive image features from scale-invariant keypoints. International Journal of Computer Vision, 60, 2 (2004), pp.91--110.

#Requirements

The codes are compiled depends on several libraries:
* OpenCV (the open source computer vision libraries - [OpenCV Homepage](http://opencv.org) )
* Vlfeat (an efficient and compatiable open source computer vision C libraries - [Vlfeat Homepage](http://www.vlfeat.org/) )
* Eigen ( a C++ template library for linear algebra - [Eigen Homepage](http://eigen.tuxfamily.org/index.php?title=Main_Page))
* C++ boost (All C++er should know)

#Compile

To build the programs by make:

$cd SIREENROOT
$make

#LICENSE
See the LICENSE File

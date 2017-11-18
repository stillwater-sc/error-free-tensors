# error-free-tensors
Posit-based Error-Free Tensor Processing

# How to build
The error-free tensor processing software is build with cmake.
Install the latest cmake [Download cmake](https://cmake.org/download)

This software depends on the universal library that contains the different unum number system implementations:
```
> git clone https://github.com/stillwater-sc/universal
```
The universal library is a pure template library without any further dependencies.

Create a CMAKE variable for UNUM_DIR pointing to the root dir of your universal library.

The error-free tensor processing also depends on BOOST and MTL4:

Download and install BOOST: [Download boost](http://www.boost.org/users/download/)

Create a CMAKE variable for Boost_INCLUDE_DIR where you installed BOOST (for example: Boost_INCLUDE_DIR=/usr/local/boost_1_65_1).

Download and install MTL4: [Download MTL4](http://simunova.com/node/145)

Create a CMAKE variable for MTL_DIR where you installed MTL (for example: MTL_DIR=/usr/local/mtl/share/mtl).

Now we are ready to build the error-free tensor software.

```
> git clone https://github.com/stillwater-sc/error-free-tensors
> cd error-free-tensors/build
> cmake ..
> make
> make test

```

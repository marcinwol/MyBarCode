# mwbarcode

mwbarcode calculates a time based bar code of set of images.

## motivation

The project is inspired by [moviebarcodes](http://moviebarcode.tumblr.com). But instead of using movie frames, I want to use images. The reason is that these days its not uncommon to have hundreds or even thousands of images taken regularly using phones or camers. And I though it would be nice to have one representation of these photos, i.e. one image representing everthing.


## Compilation and installation (Ubuntu 14.04)

#### Dependencies:
```bash
sudo apt-get install build-essential cmake libmagick++-dev libboost-filesystem1.55-dev libboost-program-options1.55-dev libboost-regex1.55-dev
```


#### Get the source code
```bash
git clone https://github.com/marcinwol/mwbarcode
cd mwbarcode
```

#### Compile
```bash
cmake -DCMAKE_INSTALL_PREFIX=/opt/mwimage
make
````
`CMAKE_INSTALL_PREFIX` specifices where the binary should be installed.


#### Install
```bash
make install 
````
 or if root required 
```bash
sudo make install 
````
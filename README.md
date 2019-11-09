[![Build Status](https://travis-ci.org/mklarqvist/tomahawk.svg?branch=master)](https://travis-ci.org/mklarqvist/tomahawk)
[![Release](https://img.shields.io/badge/Release-beta_0.7.0-blue.svg)](https://github.com/mklarqvist/tomahawk/releases)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Docs](https://img.shields.io/badge/Docs-Available-brightgreen.svg)](https://mklarqvist.github.io/tomahawk/)

# Modified Tomahawk

Allows indels, probably breaks the haplotype command but import and scalc are fine

Allows multiple variants in the same position by changing their positions to unique ones and writing a mapping file between actual positions and Tomahawk positions

![screenshot](tomahawk.png)

# Fast calculation of LD in large-scale cohorts
Tomahawk is a machine-optimized library for computing [linkage-disequilibrium](https://en.wikipedia.org/wiki/Linkage_disequilibrium) from population-sized datasets. Tomahawk permits close to real-time analysis of regions-of-interest in datasets of many millions of diploid individuals on a standard laptop. All algorithms are embarrassingly parallel and have been successfully tested on datasets with up to _10 million individuals_ using thousands of cores on hundreds of machines using the [Wellcome Trust Sanger Institute](http://www.sanger.ac.uk/) compute farm.

Tomahawk is unique in that it constructs complete haplotype/genotype contigency matrices for each comparison, perform statistical tests on the output data, and provide a framework for investigating the produced data.

## Get started

* Read the [documentation](https://mklarqvist.github.io/tomahawk/)
* [Get started](https://mklarqvist.github.io/tomahawk/tutorial/) with the CLI
* [Get started](https://mklarqvist.github.io/tomahawk/r-tutorial/) with [rtomahawk](https://github.com/mklarqvist/rtomahawk)
* [Get started](https://mklarqvist.github.io/tomahawk/docker-tutorial/) with Docker and Tomahawk

## Requirements

* Your processor should support SSE4.2 (It is supported by most Intel and AMD processors released since 2008.)
* C++11 compliant compiler (GCC is assumed)
* A Linux-like distribution is assumed by the makefile

## Installation
For Ubuntu, Debian, and Mac systems, installation is easy: just run
```bash
git clone --recursive https://github.com/mklarqvist/tomahawk
cd tomahawk
./install.sh
```
The `install.sh` file depends extensively on `apt-get`, so it is unlikely to run without extensive modifications on non-Debian-based systems.
If you do not have super-user (administrator) privileges required to install new packages on your system then run the local installation:
```bash
./install.sh local
```
When installing locally, the required dependencies are downloaded and built in the root directory. This approach will require additional effort if you intend to move the compiled libraries to a different directory.

### Contributing

Interested in contributing? Fork and submit a pull request and it will be reviewed.

### Support
We are actively developing Tomahawk and are always interested in improving its quality. If you run into an issue, please report the problem on our Issue tracker. Be sure to add enough detail to your report that we can reproduce the problem and address it. We have not reached version 1.0 and as such the specification and/or the API interfaces may change.

### Version
This is Tomahawk 0.7.0. Tomahawk follows [semantic versioning](https://semver.org/).

### Author
Marcus D. R. Klarqvist (<mk819@cam.ac.uk>)  
Department of Genetics, University of Cambridge  
Wellcome Sanger Institute


### License
Tomahawk is licensed under [MIT](LICENSE)

[htslib]:   https://github.com/samtools/htslib
[zstd]:     https://github.com/facebook/zstd
[tomahawk]: https://github.com/mklarqvist/tomahawk

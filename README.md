[![Build Status](https://travis-ci.org/mklarqvist/Tomahawk.svg?branch=master)](https://travis-ci.org/mklarqvist/Tomahawk)
[![Release](https://img.shields.io/badge/Release-beta_0.3-blue.svg)](https://github.com/mklarqvist/Tomahawk/releases)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

![screenshot](tomahawk.png)
## Fast genetics in large-scale human cohorts
Tomahawk efficiently compress genotypic data by exploiting intrinsic genetic properties and we describe algorithms to directly query, manipulate, and explore this jointly compressed representation in-place. We represent genotypic vectors as fixed-width run-length encoded (RLE) objects with the five highest bits encoding for phasing, allele A, allele B, and the remainder as the number of runs. This encoding scheme is superior to dynamic-width encoding approaches in terms of iteration speed but inferior in terms of compressibility. The word size (`uint8_t`, `uint16_t`, `uint32_t`, or `uint64_t`) of RLE entries is determined by the number of samples in the matrix during run-time. Tomahawk has three primary internal functions: 1) iterate over RLE entries; 2) divide compressed genotypic vectors into groups; 3) computing the inner product of compressed genotypic vectors.

We describe efficient algorithms to calculate genome-wide linkage disequilibrium for all pairwise alleles/genotypes in large-scale cohorts. In order to achieve speed, Tomahawk combines primarily two efficient algorithms exploiting different concepts: 1) low genetic diversity and 2) the large memory registers on modern processors. The first algorithm directly compares RLE entries from two vectors. The other transforms RLE entries to bit-vectors and use SIMD-instructions to directly compare two such bit-vectors. This second algorithm also exploits the relatively low genetic diversity within species using implicit heuristics. Both algorithms are embarrassingly parallel.

The current format specifications (v.0) for `TWK`,`TWI`,`TWO`,`TOI`, `LD`, and `TGZF`
are available [TWKv0](spec/TWKv0.pdf)

Marcus D. R. Klarqvist (<mk819@cam.ac.uk>)

### Installation instructions
For modern x86-64 CPUs with `SSE4.2` or later, just type `make` in the `build`
directory. If you see compilation errors, you most likely do not have `SSE4.2`.
At the present time, we do not support non-x86 CPUs or old CPU architecture.
```bash
git clone --recursive https://github.com/mklarqvist/Tomahawk
cd Tomahawk/build
make
```
By default, Tomahawk is compiled with aggressive optimization flags and
with native architecture-specific instructions
(`-march=native -mtune=native -ftree-vectorize -frename-registers -funroll-loops`)
and internally compiles for the most recent SIMD-instruction set available.
This could result in additional effort when submitting jobs to
computer farms/clouds with a hardware architecture that is different from the
compiled target.

### Brief usage instructions
Tomahawk comprises five primary commands: `import`, `calc`, `view`, `sort`, `index`, and `concat`.
The function `stats` have partial support: currently limited to basics for `two` files.
Executing `tomahawk` gives a list of commands with brief descriptions and `tomahawk <command>`
gives detailed details for that command.

All primary Tomahawk commands operate on the binary Tomahawk `twk` and Totempole `twi` file
format. Interconversions between `twk` and `vcf`/`bcf` is supported through the
commands `import` for `vcf`/`bcf`->`twk` and `view` for `twk`->`vcf`. Linkage
disequilibrium data is written out in `two` and `toi` format.

### Importing to Tomahawk
By design Tomahawk only operates on bi-allelic SNVs and as such filters out
indels and complex variants. Tomahawk does not support mixed phasing of genotypes
in the same variant (e.g. `0|0`, `0/1`). If mixed phasing is found in a line,
all genotypes in that line are converted to unphased. Importing a variant document (`vcf`/`bcf`)
to Tomahawk requires the `import` command.
The following command line imports a `vcf` file and outputs `outPrefix.twk` and
`outPrefix.twk.twi` and filters out variants with >20% missingness and deviate
from Hardy-Weinberg equilibrium with a probability < 0.001
```bash
tomahawk import -i file.vcf -o outPrefix -m 0.2 -h 1e-3
```

### Import-extend
If you have split up your `vcf`/`bcf` files into multiple disjoint files
(such as one per chromosome) it is possible to iteratively import and extend a `twk` file:
```bash
tomahawk import -i file.bcf -e extend.twk -m 0.2 -h 1e-3
```

### Calculating linkage disequilibrium
```bash
tomahawk calc -pdi file.twk -o output_prefix -a 5 -r 0.1 -P 0.1 -c 990 -C 1 -t 28
```

### Converting between file formats and filtering
Viewing LD data from the binary `two` file format and filtering out lines with a
Fisher's exact test P-value < 1e-4, minor haplotype frequency < 5 and have
FLAG bits `4` set
```bash
tomahawk view -i file.two -P 1e-4 -a 5 -f 4
 ```

It is possible to filter `two` output data by: 1) either start or end contig e.g.
`chr1`, 2) position in that contig `chr1:10e6-20e6`, or 3) or have a particular
contig mapping `chr1,chr2`, or 4) a particular regional mapping in both contigs
`chr1:10e3-10e6,chr2:0-10e6`
```bash
tomahawk view -i file.two chr1:10e3-10e6,chr2:0-10e6
 ```

Converting a `twk` file to `vcf`
 ```bash
tomahawk view -i file.twk -o file.vcf
```

### Sort `TWO` file
Partially sort `two` file in 500 MB chunks
```bash
tomahawk sort -i file.two -o partial.two -L 500
```

Perform k-way merge of partially sorted blocks
```bash
tomahawk sort -i partial.two -o sorted.two -M
```

### License
[MIT](LICENSE)

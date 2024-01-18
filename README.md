# Setup Dorado with rocm support

Here are the necessary modules to setup Dorado on LUMI-G.

```
git clone https://github.com/HichamAgueny/Dorado_HIP.git
```

## Add additional packages

```
cd  dorado_hip/dorado/3rdparty

git clone https://github.com/samtools/htslib.git
git clone https://github.com/gabime/spdlog.git
git clone https://github.com/Sygmei/11Zip.git
git clone https://github.com/nanoporetech/ont-minimap2.git
git clone https://github.com/Martinsos/edlib.git
git clone https://github.com/HowardHinnant/date.git

git clone https://github.com/nanoporetech/vbz_compression.git
And then $ mv vbz_compression hdf_plugins

$cd hdf_plugins/third_party/streamvbyte
And then $git clone https://github.com/lemire/streamvbyte.git

git clone --recurse-submodules https://github.com/Sygmei/11Zip.git
And then $mv 11Zip elzip

$cd 3rdparty/ont-minimap2/src/3rdparty
git clone https://github.com/lh3/minimap2.git
git clone https://github.com/GerHobbelt/pthread-win32.git
git clone https://github.com/zlib-ng/zlib-ng.git
```

## Load modules
```
ml cray-python/3.9.12.1
module load gcc-mixed
ml LUMI/23.09  partition/G
ml buildtools/23.09
```
```
ml cray-hdf5/1.12.2.7
ml zlib/1.2.13-cpeCray-23.09
ml zstd/1.5.5-cpeCray-23.09
ml Szip/2.1.1-cpeCray-23.09
ml libaec/1.0.6-cpeCray-23.09
```

```
module use /pfs/lustrep2/projappl/project_462000125/samantao-public/mymodules
ml rocm/5.7.1
```
```
source /project/project_465000096/hich/XDorado_hip/PyT2.0.1_rocm5.7_pyt3.9.12.1/bin/activate
```
```
export LD_LIBRARY_PATH=/opt/cray/pe/gcc/10.3.0/snos/lib64:$LD_LIBRARY_PATH

export LD_LIBRARY_PATH=/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/lib:LD_LIBRARY_PATH
```

**ATen** should be here: `ATEN_INCLUDE_DIR=PyT2.0.1_rocm5.7_pyt3.9.12.1/lib/python3.9/site-packages/torch/include/ATen`

### Configure & Build
```
$rm -rf build
$mkdir build
$cd build
$cmake -DCMAKE_CXX_COMPILER=CC -S .. -B .
$cmake --build . --config Release -j 4
```

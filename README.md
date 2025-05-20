# Dorado

This is a HIP version of Dorado. **The project is still ongoing** and is not fully ported to HIP. The original version is based on CUDA and can be found [here](https://github.com/nanoporetech/dorado/tree/master). In short Dorado is a high-performance, easy-to-use, open source basecaller for Oxford Nanopore reads.

# Setup Dorado for LUMI-G system (with rocm support)
Here is a description on how to setup Dorado-HIP on LUMI-G.
```
cd /project/project_4650000XX/
git clone https://github.com/HichamAgueny/Dorado_HIP.git
```
Add additional packages
```
cd  Dorado_HIP/dorado_hip/dorado/3rdparty
```
```
git clone https://github.com/samtools/htslib.git
git clone https://github.com/gabime/spdlog.git
git clone https://github.com/Sygmei/11Zip.git
git clone https://github.com/nanoporetech/ont-minimap2.git
git clone https://github.com/Martinsos/edlib.git
git clone https://github.com/HowardHinnant/date.git
git clone https://github.com/yhirose/cpp-httplib.git
git clone https://github.com/NVIDIA/NVTX.git
git clone https://github.com/BlueBrain/HighFive.git
gite clone https://github.com/bloomen/cxxpool.git
git clone https://github.com/p-ranav/indicators.git
git clone https://github.com/ToruNiina/toml11.git

git clone https://github.com/nanoporetech/vbz_compression.git
And then $ mv vbz_compression hdf_plugins

$cd hdf_plugins/third_party/streamvbyte
$rm -rf streamvbyte
And then $git clone https://github.com/lemire/streamvbyte.git

$cd dorado_hip/dorado/3rdparty
git clone --recurse-submodules https://github.com/Sygmei/11Zip.git
And then $mv 11Zip elzip

$cd 3rdparty/ont-minimap2/src/3rdparty
git clone https://github.com/lh3/minimap2.git
git clone https://github.com/GerHobbelt/pthread-win32.git
git clone https://github.com/zlib-ng/zlib-ng.git
```

## Setup PyTorch-rocm
In a virtual environment
```
cd /project/project_4650000XX/
```
```
ml cray-python/3.9.12.1
module use /pfs/lustrep2/projappl/project_462000125/samantao-public/mymodules
ml rocm/5.7.1
```
```
python -m venv PyT2.3.0_rocm5.7_pyt3.9.12.1
source PyT2.3.0_rocm5.7_pyt3.9.12.1/bin/activate
pip install --pre torch torchvision torchaudio --extra-index-url https://download.pytorch.org/whl/nightly/rocm5.7/

deactivate
```

## Build Dorado
Load modules
```
ml LUMI/23.09  partition/G
ml cray-python/3.9.12.1
module load gcc-mixed
ml buildtools/23.09
module use /pfs/lustrep2/projappl/project_462000125/samantao-public/mymodules
ml rocm/5.7.1
```
And some libraries

```
ml cray-hdf5/1.12.2.7
ml zlib/1.2.13-cpeCray-23.09
ml zstd/1.5.5-cpeCray-23.09
ml Szip/2.1.1-cpeCray-23.09
ml libaec/1.0.6-cpeCray-23.09
```
Activate the virtual env.
```
source /project/project_4650000XX/PyT2.3.0_rocm5.7_pyt3.9.12.1/bin/activate
```

And make some paths available 
```
export LD_LIBRARY_PATH=/opt/cray/pe/gcc/10.3.0/snos/lib64:$LD_LIBRARY_PATH

export LD_LIBRARY_PATH=/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/lib:LD_LIBRARY_PATH
```

### Configure & Build
```
$rm -rf build
$mkdir build
$cd build
$cmake -DCMAKE_CXX_COMPILER=CC -S .. -B .
```

During the configuration an error message related to missing `htslib` lib is printed out. To resolve the error run the following  
```
$cd Dorado_HIP/dorado_hip/build/htslib_build/htslib
$git submodule update --init --recursive
$./configure
```
After the configuratioon step is completed:
```
$cd Dorado_HIP/dorado_hip/build
$cmake --build . --config Release -j 4
```

## Build aws-ofi-rccl (communication between GPUs) plugin with EasyBuild

Building `aws-ofi-rccl` is only necessary for running on multiple GPUs.

**Step 1:** Specify the path for EasyBuild
```
$ export EBU_USER_PREFIX=/project/project_4650000XX/EasyBuild
```

**Step 2:** Load necessary modules
```
$ module load LUMI/22.12 partition/G
$ module load rocm/5.3.2
$ module load EasyBuild-user
```
And if it is needed: `export PYTHONIOENCODING=utf-8`

**Step 3:** Install aws-ofi-rccl plugin
```
$ eb aws-ofi-rccl-1.4.0-cpeGNU-22.12-rocm-5.3.2.eb -r
```

## Licence and Copyright

(c) 2023 Oxford Nanopore Technologies PLC.

Dorado is distributed under the terms of the Oxford Nanopore
Technologies PLC.  Public License, v. 1.0.  If a copy of the License
was not distributed with this file, You can obtain one at
http://nanoporetech.com

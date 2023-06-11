# Building releases

* Update CHANGELOG.md
* Tag the release commit, e.g. ```git tag 0.1.0```

## Ubuntu

```bash
cd packaging
make DIST=focal deb
make DIST=jammy deb
make DIST=kinetic deb

cd _BUILD/ubuntu/focal
dput ppa:bkryza/clang-uml *.changes

cd _BUILD/ubuntu/jammy
dput ppa:bkryza/clang-uml *.changes

cd _BUILD/ubuntu/kinetic
dput ppa:bkryza/clang-uml *.changes

```

## Fedora

```bash
cd clang-uml
make fedora_36
make fedora_37
find packaging/_BUILD/fedora
```

## Anaconda

```bash
docker run --rm -v $PWD:$PWD -it continuumio/miniconda3 bash
conda install conda-build make anaconda-client
cd packaging
make CONDA_TOKEN=<TOKEN> conda
```

## Windows

First build release configuration using `cmake` and `msbuild` according
to the [documentation](../docs/installation.md#visual-studio-native-build).

```bash
cd packaging
.\make_installer.ps1
ls .\_BUILD\windows\clang-uml-0.3.7-win64.exe
```
# xcShell - build your own wheel
[KANBAN](https://github.com/users/superxcgm/projects/4)

## Install Dependencies
```shell
mkdir build
cd build
conan install ..
```

## Build
```shell
cd build
cmake .. && cmake --build .
```

## Run Test
```shell
cd build
./xcShell_test
```
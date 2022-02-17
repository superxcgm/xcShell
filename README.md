# xcShell - build your own wheel
![](https://github.com/superxcgm/xcShell/actions/workflows/ci.yml/badge.svg)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=superxcgm_xcShell&metric=coverage)](https://sonarcloud.io/summary/new_code?id=superxcgm_xcShell)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=superxcgm_xcShell&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=superxcgm_xcShell)

[KANBAN](https://github.com/users/superxcgm/projects/4)

## Usage
1. `git clone https://github.com/superxcgm/xcShell.git`
2. `cd xcShell`
3. `./auto/build.sh`
4. `cd build && make install`
5. `xcShell`

## Build
```shell
./auto/build.sh
```

## Format Your Code
```shell
./auto/format.sh
```

## Run
```shell
./auto/run.sh
```

## Check Code Style
```shell
./auto/lint.sh
```

## Unit Test
```shell
./auto/test.sh
```

## Functional Test
### Run
```shell
./auto/functional_test.sh run
```

### Add a New Test Suite
```shell
./auto/functional_test.sh generate xxx
# For example: ./auto/functional_test.sh generate pipe
```
This command will generate empty file `xxx.functional_test.in` and `xxx.functional_test.expected.out` in dir `auto/data`
. And then you can write your test in `xxx.functional_test.in`, run functional test and check output of you test. It
should be located in `/tmp/xcShell_functional_test/xxx.functional_test.out`.

You can save output by `./auto/functional_test.sh apply xxx` if the output file match you expect.

## Contribution Guideline
[Link](https://github.com/superxcgm/xcShell/wiki/Contribution-Guideline)
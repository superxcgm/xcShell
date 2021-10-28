#!/bin/bash

run () {
  if ! ./auto/build.sh ; then
    echo "xcShell build failed"
    exit 1
  fi
  test_dir=/tmp/xcShell_functional_test
  rm -rf $test_dir
  mkdir -p $test_dir
  cp auto/data/* $test_dir
  cp build/xcShell $test_dir

  cd $test_dir

  ret_code=0

  for input_file in *.functional_test.in
  do
    echo "Run test $input_file"
    base=$(basename "$input_file" .in)
    before_file=$base.before.sh
    output_file=$base.out
    expected_file=$base.expected.out
    diff_file=$base.diff
    cmd=""
    if [ -e "$before_file" ] ; then
      chmod +x "$before_file"
      cmd="source ./$before_file && "
    fi
    #  echo $input_file
    #  echo $output_file
    #  echo $expected_file
    cmd="${cmd} ./xcShell --no-load-config < $input_file > $output_file 2>&1"
    /bin/bash -c "$cmd"
    if diff "$output_file" "$expected_file" > "$diff_file" ; then
      echo -e "  Test $input_file \033[32mpass\033[0m. "
      rm "$diff_file"
    else
      echo -e "  Test $input_file \033[31mfailed\033[0m. "
      cat $diff_file
      ret_code=1
    fi
  done

  exit $ret_code
}

usage() {
  echo "Usage: "
  echo "  # Run All Functional Test"
  echo "  ./auto/functional_test.sh run"
  echo ""
  echo "  # Generate Test Files"
  echo "  ./auto/functional_test.sh generate TEST_SUITE_NAME"
  echo ""
  echo "  # Apply Test Output as Expected"
  echo "  ./auto/functional_test.sh apply TEST_SUITE_NAME"
  exit 1
}

generate() {
  touch "auto/data/${1}.functional_test.in"
  touch "auto/data/${1}.functional_test.expected.out"
}

apply() {
  cp "/tmp/xcShell_functional_test/${1}.functional_test.out" "auto/data/${1}.functional_test.expected.out"
}

case "$1" in
  "run")
    run
    ;;
  "generate")
    generate "$2"
    ;;
  "apply")
    apply "$2"
    ;;
  *)
    usage
    ;;
esac

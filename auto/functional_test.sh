#!/bin/bash

run () {
  if ! ./auto/build.sh ; then
    echo "xcShell build failed"
    exit 1
  fi

  rm -rf /tmp/xcShell_functional_test
  mkdir -p /tmp/xcShell_functional_test
  cp auto/data/*.functional_test.in /tmp/xcShell_functional_test
  cp auto/data/*.functional_test.expected.out /tmp/xcShell_functional_test
  cp build/xcShell /tmp/xcShell_functional_test

  cd /tmp/xcShell_functional_test

  ret_code=0

  for input_file in *.functional_test.in
  do
    echo "Run test $input_file"
    output_file=$(basename "$input_file" .in).out
    expected_file=$(basename "$input_file" .in).expected.out
    diff_file=$(basename "$input_file" .in).diff
    #  echo $input_file
    #  echo $output_file
    #  echo $expected_file
    ./xcShell --no-load-config --cd-history-file=/tmp/cd_history_test.txt< "$input_file" > "$output_file" 2>&1
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

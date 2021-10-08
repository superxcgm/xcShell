#!/bin/bash

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

for input_file in *.functional_test.in
do
  echo "Run test $input_file"
  output_file=$(basename "$input_file" .in).out
  expected_file=$(basename "$input_file" .in).expected.out
  diff_file=$(basename "$input_file" .in).diff
  #  echo $input_file
  #  echo $output_file
  #  echo $expected_file
  ./xcShell < "$input_file" > "$output_file" 2>&1
  if diff "$output_file" "$expected_file" > "$diff_file" ; then
    echo -e "Test $input_file \e[32mpass. "
    rm "$diff_file"
  else
    echo -e "Test $input_file \e[31mfailed. You can checkout $diff_file for more detail"
  fi
done

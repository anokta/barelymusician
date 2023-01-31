#!/bin/sh

# Compares BarelyMusician API function declarations in `barelymusician.h`
# against the corresponding definitions in `barelymusician.cpp`.

main() {
  h_file_path="barelymusician/barelymusician.h"
  declarations=$(awk '/^BARELY_EXPORT/,/\(/' "${h_file_path}" |
                 grep -o "Barely[A-Za-z]*_[A-Za-z]*(" |
                 sed 's/.$//')

  cpp_file_path="barelymusician/barelymusician.cpp"
  definitions=$(grep -o "Barely[A-Za-z]*_[A-Za-z]*(" "${cpp_file_path}" |
                sed 's/.$//')

  diff=$(echo ${declarations[@]} ${definitions[@]} | tr ' ' '\n' | sort |
         uniq -u)
  if (( ${#diff[@]} )); then
    echo "${diff}"
    exit 1
  fi
}
main

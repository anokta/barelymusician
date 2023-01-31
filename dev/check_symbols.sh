#!/bin/sh
#
# Compares the BarelyMusician API function declarations in `barelymusician.h` against the
# corresponding definitions in `barelymusician.cpp`.

readonly WORKSPACE="$(bazel info workspace)"

readonly API_DIR="${WORKSPACE}/barelymusician"
readonly API_H_PATH="${API_DIR}/barelymusician.h"
readonly API_CPP_PATH="${API_DIR}/barelymusician.cpp"

main() {
  set -e

  echo "Parsing ${API_H_PATH}..."
  local declarations=$(
    sed -n '{N; s/^BARELY_EXPORT\s\+\w\+\s\+\(Barely[A-Za-z]\+_[A-Za-z]\+\)(.*/\1/p; D}' \
      "${API_H_PATH}"
  )
  echo "${declarations}"
  LC_COLLATE=C sort -uc <<<${declarations[*]}

  echo "Parsing ${API_CPP_PATH}..."
  local definitions=$(
    sed -n '{N; s/^\w\+\s\+\(Barely[A-Za-z]\+_[A-Za-z]\+\)(.*\+/\1/p; D}' "${API_CPP_PATH}"
  )
  echo "${definitions}"
  LC_COLLATE=C sort -uc <<<${definitions[*]}

  echo "Comparing symbols..."
  local diff=$(echo ${declarations[@]} ${definitions[@]} | tr ' ' '\n' | sort | uniq -u)
  if [ ! -z "${diff}" ]; then
    echo "Mismatching symbols were found:"
    echo "${diff[@]}"
    exit 1
  fi

  echo "Done!"
}

main

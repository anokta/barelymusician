#!/bin/bash
#
# Compares the BarelyMusician API function declarations in `barelymusician.h` against the
# corresponding definitions in `barelymusician.cpp`.
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
readonly API_DIR="${SCRIPT_DIR}/../barelymusician"
readonly API_H_PATH="${API_DIR}/barelymusician.h"
readonly API_CPP_PATH="${API_DIR}/barelymusician.cpp"

readonly BLACK="\e[30m"
readonly RED="\e[31m"
readonly GREEN="\e[32m"
readonly RESET="\e[0m"

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
    echo -e "${RED}Mismatching symbols were found:"
    echo -e "${BLACK}${diff[@]}"
    exit 1
  fi

  echo -e "${GREEN}Done!${RESET}"
}

main

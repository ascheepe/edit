#! /bin/sh
limit=${1:-80}

expand | awk -v limit="$limit" 'length($0) >= limit { print NR, $0 }'

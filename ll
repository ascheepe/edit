#! /bin/sh
maxlen=${1:-80}

expand | awk -v maxlen="$maxlen" 'length($0) >= maxlen { print NR, $0 }'

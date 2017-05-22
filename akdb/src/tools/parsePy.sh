 #!/bin/sh

for var in "$@"
do
    grep -E "#" $var >> "all_comments.tmp"
done
#!/bin/bash
#!/bin/bash

a=`find ./ -name "*.c"  | xargs cat | grep -v ^$ | wc -l`
b=`find ./ -name "*.h"  | xargs cat | grep -v ^$ | wc -l`

echo $[$a + $b]

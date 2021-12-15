#!/bin/bash

echo Removing all .so and  files...

find nifs -type f -name '*.so' -exec rm {} +
rm build -rf

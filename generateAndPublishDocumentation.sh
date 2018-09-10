#!/bin/bash

# This script generates the doxygen documentation and pushes the result to the gh-pages branch

echo "--- generate doxygen documentation using cmake"
mkdir -p build
cd build
cmake ..
make doc

cd ..
echo "--- switch to gh-pages branch"
git checkout gh-pages
echo "--- remove old documentation"
rm -rf html
echo "--- copy new documentation"
cp -r build/APIDocu/html .

echo "--- commit new documentation"
git add .
git commit -m "update doxygen documentation"
git push origin gh-pages

echo "--- switch bach to previous branch"
git checkout -

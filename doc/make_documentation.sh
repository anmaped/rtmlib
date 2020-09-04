#!/bin/bash

set +e

# check if we have doxygen installed
! [ -x "$(command -v doxygen)" ] && exit 1

# run doxygen to generate xml files
doxygen config >/dev/null

# check if we have perl and cpan installed
! [ -x "$(command -v perl)" ] && exit 1

! [ -x "$(command -v cpan)" ] && exit 1

# this part will run if cpan and perl is available

# install cpan packages if available
sudo cpan File::Spec GraphViz2 Hash::FieldHash lib::abs Moose namespace::autoclean XML::Rabbit >/dev/null

# compile doxygraph
cd doxygraph
perl Makefile.PL
make
cd -

# run doxygraph
perl doxygraph/doxygraph _out/xml/index.xml rtmlib_class_diagram.dot

# re-run doxygen to include the new generated doxygraph
doxygen config >/dev/null

# clean-up
cd doxygraph
make clean
cd -






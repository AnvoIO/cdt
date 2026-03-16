#! /bin/bash

# Homebrew bottle generation for CDT
# Supports both Intel and Apple Silicon

NAME="${PROJECT}-${VERSION}.bottle"

mkdir -p ${PROJECT}/${VERSION}/opt/cdt/lib/cmake

PREFIX="${PROJECT}/${VERSION}"
SPREFIX="\/usr\/local"
SUBPREFIX="opt/${PROJECT}"
SSUBPREFIX="opt\/${PROJECT}"

export PREFIX
export SPREFIX
export SUBPREFIX
export SSUBPREFIX

. ./generate_tarball.sh ${NAME}

hash=$(openssl dgst -sha256 ${NAME}.tar.gz | awk 'NF>1{print $NF}')

echo "class Cdt < Formula
   # typed: false
   # frozen_string_literal: true

   homepage \"${URL}\"
   revision 0
   url \"https://github.com/Anvo-Network/cdt/archive/v${VERSION}.tar.gz\"
   version \"${VERSION}\"

   depends_on \"cmake\" => :build
   depends_on \"automake\" => :build
   depends_on \"libtool\" => :build
   depends_on \"wget\" => :build
   depends_on \"gmp\" => :build
   depends_on \"doxygen\" => :build
   depends_on \"graphviz\" => :build
   depends_on xcode: :build

   bottle do
      root_url \"https://github.com/Anvo-Network/cdt/releases/download/v${VERSION}\"
      sha256 arm64_sonoma: \"${hash}\"
   end
   def install
      raise \"Error, only supporting binary packages at this time\"
   end
end
__END__" &> cdt.rb

rm -r ${PROJECT} || exit 1

#!/bin/csh

foreach d (`ls -d [CDNS]*`)
cd $d
make clean
make
cd ..
end

cd SPECTRAL_SNR/lin_rms
make clean
make
cd ../..

foreach d (`ls -d FTA/*`)
cd $d
make clean
make
cd ../..
end

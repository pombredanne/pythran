if $PEP8;
then
  PYTHONPATH=$PWD:$PYTHONPATH py.test pythran --pep8 -m pep8;
else
  printf "[compiler]\nCXX=$CXX\nCC=$CC\ncflags=-std=c++11 $CXXFLAGS -w\nldflags=$CXXFLAGS\n" > ~/.pythranrc;
  OMP_NUM_THREADS=4 PYTHONPATH=$PWD:$PYTHONPATH py.test -v $TESTCASE;
fi

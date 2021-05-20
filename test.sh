BUILD_SIM_PATH="./spimsimulator-build"

sh make.sh

printf "test file: %s\n\n" $1

cd $BUILD_SIM_PATH/spim
./spim -f ../../test/$1

echo "\n----------- TEST END ------------------"

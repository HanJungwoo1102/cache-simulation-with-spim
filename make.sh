ORIGIN_SIM_PATH="./spimsimulator-origin"
BUILD_SIM_PATH="./spimsimulator-build"

rm -r $BUILD_SIM_PATH

cp -r $ORIGIN_SIM_PATH $BUILD_SIM_PATH

cp ./src/cpu/* $BUILD_SIM_PATH/CPU/
cp ./src/Makefile $BUILD_SIM_PATH/spim/

cd $BUILD_SIM_PATH/spim
make

echo "----------- MAKE END ------------------"

sudo apt update && sudo apt-get install -y wget lsb-release software-properties-common
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg > /dev/null && sudo apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main"
sudo apt-get install -y g++ cmake  libboost-dev  libomp-dev libtbb-dev libgtest-dev libnlopt-dev libssl-dev g++-10
git clone https://github.com/google/benchmark.git
cd benchmark && mkdir build && cd build && cmake .. -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON && make -j && sudo make install
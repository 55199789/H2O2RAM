# H2O2RAM: A High-performance Hierarchical Doubly Oblivious RAM

## Setup TDX

Setting up a TDX environment requires a combination of hardware and software:

- Intel's Sapphire Rapids (SPR) CPU
- Host OS kernel and guest OS kernel
- Virtualization components such as Qemu, OVMF, and Kata.

Adapting these components is somewhat challenging. One may follow the instructions in [Intel TDX Tools](https://github.com/intel/tdx-tools) and [Intel TDX](https://github.com/canonical/tdx) to setup the envorinment. While it is fortunate that cloud computing providers are gradually offering confidential computing services, which alleviates the need for the adaptation processes. Moreover, we provide our 64-core, 256GB trusted virtual machine startup script and host-side verification script to assist in the initialization and verification of the TEE environment.

The specific commands that we utilized are as follows.

1. Check your host environment.
```bash
./25.tdx_host_check.sh
```
The output should include
```
[   22.593272] tdx: TDX module initialized.
```

2. <span id = 'prep'> Build your `demo.raw` by following [this blog](https://blog.programster.org/create-ubuntu-20-kvm-guest-from-cloud-image). Alternatively, you also have the option to download our pre-configured VM image directly as follows. </span>

```bash
wget https://zenodo.org/records/15000727/files/H2O2RAM.zip && unzip demo.zip
```

3. Start your TD VM. The default username and password of the prepared image are ```root``` and ```123456```.

```bash
./start.sh
```

4. Check the memory encryption is active like the screenshot below, which means TD VM has benn successfully setup.

![the screenshot of successfully setup TDX](./screenshot.jpg)

## Compile

If you choose to prepare your own VM image in [step 2](#prep), you need to pull and compile our project by

```bash
git clone https://github.com/55199789/H2O2RAM.git
cd H2O2RAM
bash ./setup.sh
mkdir build && cd build
cmake .. && make -j
```

Otherwise, you can simply login to the VM and enter the directory ```~/H2O2RAM```. 

## Benchmark

Note that the full benchmark takes a lot of time, and it will fail if the running TDX has limited private memory.

```bash
../bin/ORAMBenchmark --benchmark_repetitions=10
```

Instead, we recommend to run the Python scripts that will save experimental results one by one: 
```bash 
cd benchmarks
python3 oram_exp_script.py &
python3 omap_exp_script.py &
python3 osssp_exp_script.py &
```

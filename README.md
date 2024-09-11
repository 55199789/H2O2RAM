# H2O2RAM: A High-performance Hierarchcial Doubly Oblivious RAM

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

2. Build your `demo.img` like [this blog](https://blog.programster.org/create-ubuntu-20-kvm-guest-from-cloud-image).

3. Start your TD VM.

```bash
./start.sh
```

4. Check the memory encryption is active like the screenshot below, which means TD VM has benn successfully setup.

![the screenshot of successfully setup TDX](./screenshot.jpg)

## Compile

```bash
git clone https://github.com/55199789/H2O2RAM.git
cd ORAM
mkdir build && cd build
cmake .. && make -j
```

## Benchmark

Note that the full benchmark takes a lot of time, and it will fail if the running TDX has limited private memory.

```bash
../bin/ORAMBenchmark --benchmark_repetitions=10
```

Instead, we recommand to run the Python scripts that will save experimental results one by one: 
```bash 
cd benchmarks
python3 oram_exp_script.py &
python3 omap_exp_script.py &
python3 osssp_exp_script.py &
```
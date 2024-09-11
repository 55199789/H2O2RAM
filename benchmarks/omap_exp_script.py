import subprocess
import os 
import json

process = subprocess.Popen(f"(cd ../build && cmake .. && make -j)", shell=True)
process.wait()
for n_base in range(8, 32):
    n = 2**n_base 
    file_name = f"result/results_omap_{n}.json"
    if os.path.exists(file_name):
        with open(file_name) as f:
            try:
                json.load(f)
                exit(0)
            except:
                pass
    process = subprocess.Popen(f"../bin/ORAMBenchmark  --benchmark_out={file_name} \
                                    --benchmark_filter=OMapDataFixture/OMap/{n}/* --benchmark_repetitions=10", shell=True)
    print(process)
    process.wait()
import subprocess
import os 
import json

process = subprocess.Popen(f"(cd ../build && cmake .. && make -j)", shell=True)
process.wait()
if os.path.exists(f"result/results_sssp.json"):
    with open(f"result/results_sssp.json") as f:
        try:
            json.load(f)
            exit(0)
        except:
            pass
process = subprocess.Popen(f"../bin/ORAMBenchmark  --benchmark_out=result/results_sssp.json \
                                --benchmark_filter=OSSSPDataFixture/ --benchmark_repetitions=10", shell=True)
print(process)
process.wait()
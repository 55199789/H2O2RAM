import subprocess
import os 
import math
import json
# first group, time vs different n and block size 
process = subprocess.Popen(f"(cd ../build &&  cmake ..  && make -j)", shell=True)
process.wait()
block_sizes = [16,32,64,128,256,512,1024,4096,8192,16384,32768,65536]
# for n in ns:
for b in block_sizes[::-1]:
    cnt_ub = 31 - int(math.log2(b))
    for n_base in range(8, cnt_ub+1):
        n = 2**n_base 
        file_name = f"result/results_{n}_{b}_64.json"
        if os.path.exists(file_name):
            with open(file_name) as f:
                try:
                    json.load(f)
                    continue
                except:
                    pass
        cmd = f"../bin/ORAMBenchmark  --benchmark_out={file_name} \
                                    --benchmark_filter=ORAMDataFixture{b}/ORAM/{n}/process_time/* --benchmark_repetitions=10"
        print(cmd)
        process = subprocess.Popen(cmd, shell=True)
        try:
            process.wait()
        except Exception as e:
            print(e)
    # input()

block_sizes = [16, 256]
# for n in ns:
thread_nums = [1,2,4,8,16,32]
for t in thread_nums:
    for b in block_sizes:
        cnt_ub = 33 - int(math.log2(b))
        for n_base in range(8, cnt_ub+1):
            n = 2**n_base 
            file_name = f"result/results_{n}_{b}_{t}.json"
            if os.path.exists(file_name):
                try:
                    json.load(f)
                    continue
                except:
                    pass
            process = subprocess.Popen(f"taskset -c 0-{t-1} ../bin/ORAMBenchmark  --benchmark_out={file_name} \
                                        --benchmark_filter=ORAMDataFixture{b}/ORAM/{n}/process_time/*", shell=True)
            print(process.args)
            try:
                process.wait()
            except Exception as e:
                print(e)

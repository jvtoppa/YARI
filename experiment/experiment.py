import subprocess
import os
from pathlib import Path
import re

def peak_memory(massif_file):
    if not os.path.exists(massif_file):
        return None
    peak = 0
    try:
        with open(massif_file, 'r') as f:
            for line in f:
                if line.startswith("mem_heap_B="):
                    val = int(line.split('=')[1])
                    if val > peak:
                        peak = val
        return peak
    except Exception:
        return None

def experiment():
    base_dataset_dir = '../datasets/'
    base_report_dir = 'memory_reports'
    
    with open("experiments.csv", "w") as f:
        f.write("Filename, Peak Memory (MiB), Compression time (ms), Encoding time (ms), Bitsize, No. of rules, CFG size, Data Type\n")
        
        for root, dirs, files in os.walk(base_dataset_dir):
            for filename in files:
                dataset_path = os.path.join(root, filename)
                category = "non-repetitive" if "non-repetitive" in root else "repetitive" if "repetitive" in root else "unknown"
                
                relative_path = os.path.relpath(root, base_dataset_dir)
                report_folder = os.path.join(base_report_dir, relative_path)
                os.makedirs(report_folder, exist_ok=True)
                report_file = os.path.join(report_folder, f"massif.out.{filename}")

                print(f"Profiling Memory: {filename}")
                try:
                    cmd_valgrind = f"valgrind --tool=massif --error-exitcode=1 --massif-out-file={report_file} ../build/repair < {dataset_path}"
                    result = subprocess.run(cmd_valgrind, capture_output=True, shell=True, timeout=1800)
                    
                    if result.returncode != 0:
                        peak_mib = "FAILED/OOM"
                    else:
                        bytes_val = peak_memory(report_file)
                        peak_mib = f"{(bytes_val / 1024 / 1024):.3f}" if bytes_val is not None else "FILE_ERROR"
                except subprocess.TimeoutExpired:
                    peak_mib = "TIMEOUT"
                except Exception as e:
                    peak_mib = f"ERROR_{type(e).__name__}"

                time_result = subprocess.run(f"../build/repair < {dataset_path}",capture_output=True,shell=True,text=True)
                comp_time = "FAILED"
                rule_history_size = None
                compressed_size = None

                if time_result.returncode == 0:
                    output = time_result.stdout.strip()

                    rule_match = re.search(r"Rule History size:\s*(\d+)", output)
                    size_match = re.search(r"Compressed text size:\s*(\d+)", output)
                    time_match = re.search(r"([\d.]+)\s*ms", output)

                    if rule_match:
                        rule_history_size = int(rule_match.group(1))

                    if size_match:
                        compressed_size = int(size_match.group(1))

                    if time_match:
                        comp_time = f"{time_match.group(1)} ms"

                print(f"Profiling bitsize: {filename}: ")
                bitsize_result = subprocess.run(f"../build/encodeCFG", capture_output=True, shell=True, text=True)
                file_path = Path(f"../output/compressed_cfg.bin")
                time_match_bitsize = None
                bitsize_time = "FAILED"
                if bitsize_result.returncode == 0:
                    output = bitsize_result.stdout.strip()
                    print("enters")
                    time_match_bitsize = re.search(r"Time taken:\s*([\d.]+)", output)
                    if time_match_bitsize:
                        bitsize_time = float(time_match_bitsize.group(1))
                bitsize = file_path.stat().st_size * 8
                
                log_line = f"{filename}, {peak_mib}, {comp_time}, {bitsize_time}, {bitsize}, {int(rule_history_size / 2)}, {compressed_size}, {category}"
                f.write(log_line + "\n")
                f.flush()
                print(f"Result: {log_line}\n")

if __name__ == "__main__":
    experiment()
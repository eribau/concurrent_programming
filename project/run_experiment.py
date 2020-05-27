import statistics
import subprocess
import csv
from pathlib import Path

data_dir = Path("data/")

numBodies = [120, 180, 240] # number of bodies
numSteps = 208000 # 113800 # number of steps
far = 0.25
numWorkers = [1, 2, 3, 4]

testIters = 5 # how many times to perform each test

results = {
            "nSqr_seq":[],
            "nSqr_par":[],
            "barnesHut_seq":[],
            "barnesHut_par":[]
        }

counter = 1
# For nSqr_seq
for n in numBodies:
    temp_results = []
    for i in range(testIters):
        result = subprocess.run(["./nSqr_seq", f'{n}', f'{numSteps}'], stdout=subprocess.PIPE)
        temp_results.append(float(result.stdout.decode("utf-8")))
        print("nSqr_seq "  + str(n) + " " + str(counter))
        counter += 1

    results["nSqr_seq"].append(statistics.median(temp_results))

# For barnesHut_seq
for n in numBodies:
    temp_results = []
    for i in range(testIters):
        result = subprocess.run(["./barnesHut_seq", f'{n}', f'{numSteps}', f'{far}'], stdout=subprocess.PIPE)
        temp_results.append(float(result.stdout.decode("utf-8")))
        print("barnesHut_seq "  + str(n) + " " + str(counter))
        counter += 1

    results["barnesHut_seq"].append(statistics.median(temp_results))

# For nSqr_par
for n in numBodies:
    for workers in numWorkers:
        temp_results = []
        for i in range(testIters):
            result = subprocess.run(["./nSqr_par", f'{n}', f'{numSteps}', f'{workers}'], stdout=subprocess.PIPE)
            temp_results.append(float(result.stdout.decode("utf-8")))
            print("nSqr_par " + str(n) + " " + str(counter))
            counter += 1

        results["nSqr_par"].append(statistics.median(temp_results))

# For barnesHut_par
for n in numBodies:
    for workers in numWorkers:
        temp_results = []
        for i in range(testIters):
            result = subprocess.run(["./barnesHut_par", f'{n}', f'{numSteps}', f'{far}', f'{workers}'], stdout=subprocess.PIPE)
            temp_results.append(float(result.stdout.decode("utf-8")))
            print("barnesHut_par "  + str(n) + " " + str(counter))
            counter += 1

        results["barnesHut_par"].append(statistics.median(temp_results))

with open(data_dir/"raw_data.csv", "w") as file:
    csv_writer = csv.writer(file, delimiter=',')
    for test in results:
        csv_writer.writerow(results[test])

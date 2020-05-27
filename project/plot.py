#!/usr/bin/env python3

import os
import csv
import numpy as np
import matplotlib.ticker as ticker
import matplotlib.pyplot as plt
from pathlib import Path

def main():
    data_dir = Path("data/")

    labels = ["nSqr_seq", "nSqr_par", "barnesHut_seq", "barnesHut_par"]
    nSqr_seq = []
    nSqr_par = []
    barnesHut_seq = []
    barnesHut_par = []
    with open(data_dir/"raw_data.csv", 'r') as file:
        csv_reader = csv.reader(file, delimiter=',')

        for idx, row in enumerate(csv_reader):
            if(idx == 0):
                nSqr_seq = row
            if(idx == 1):
                nSqr_par = row
            if(idx == 2):
                barnesHut_seq = row
            if(idx == 3):
                barnesHut_par = row

    nSqr_seq = [float(i) for i in nSqr_seq]
    nSqr_par = [float(i) for i in nSqr_par]
    barnesHut_seq = [float(i) for i in barnesHut_seq]
    barnesHut_par = [float(i) for i in barnesHut_par]

    font = {'family' : 'normal',
        'weight' : 'bold',
        'size'   : 22}

    plt.rc('font', **font)

    y_pos = np.arange(len(labels))
    y_240 = [nSqr_seq[2], nSqr_par[11], barnesHut_seq[2], barnesHut_par[11]]

    fig1, ax1 = plt.subplots()
    ax1.bar(y_pos, y_240, align='center')
    ax1.set_xticks(y_pos)
    ax1.set_xticklabels(labels)

    width = 0.2
    y2_pos = np.arange(3)
    fig2, ax2 = plt.subplots()
    bar1 = ax2.bar(y2_pos, [nSqr_seq[0], nSqr_seq[1], nSqr_seq[2]], width)
    bar2 = ax2.bar(y2_pos+width, [nSqr_par[3], nSqr_par[7], nSqr_par[11]], width)
    bar3 = ax2.bar(y2_pos+width*2, [barnesHut_seq[0], barnesHut_seq[1], barnesHut_seq[2]], width)
    bar4 = ax2.bar(y2_pos+width*3, [barnesHut_par[3], barnesHut_par[7], barnesHut_par[11]], width)
    ax2.set_xticks(y2_pos+width/2)
    ax2.set_xticklabels(['120', '180', '240'])
    ax2.set_ylabel("Seconds")
    ax2.set_xlabel("Number of Bodies")
    ax2.set_title("")
    ax2.legend((bar1[0], bar2[0], bar3[0], bar4[0]), ('Brute-force seq', 'Brute-force par', 'Barnes-Hut seq', 'Barnes-Hut par'))

    width = 0.2
    y3_pos = np.arange(3)
    fig3, ax3 = plt.subplots()
    bar12 = ax3.bar(y3_pos, [barnesHut_seq[0], barnesHut_seq[1], barnesHut_seq[2]], width, color='green')
    bar22 = ax3.bar(y3_pos+width, [barnesHut_par[3], barnesHut_par[7], barnesHut_par[11]], width, color='red')
    ax3.set_xticks(y3_pos+width/2)
    ax3.set_xticklabels(['120', '180', '240'])
    ax3.set_ylabel("Seconds")
    ax3.set_xlabel("Number of Bodies")
    ax3.set_title("")
    ax3.legend((bar12[0], bar22[0]), ('Barnes-Hut seq', 'Barnes-Hut par 4 workers'))

    width = 0.2
    y4_pos = np.arange(3)
    fig4, ax4 = plt.subplots()
    bar12 = ax4.bar(y4_pos, [barnesHut_seq[0], barnesHut_seq[1], barnesHut_seq[2]], width, color='green')
    bar22 = ax4.bar(y4_pos+width, [barnesHut_par[0], barnesHut_par[4], barnesHut_par[8]], width, color='red')
    ax4.set_xticks(y4_pos+width/2)
    ax4.set_xticklabels(['120', '180', '240'])
    ax4.set_ylabel("Seconds")
    ax4.set_xlabel("Number of Bodies")
    ax4.set_title("")
    ax4.legend((bar12[0], bar22[0]), ('Barnes-Hut seq', 'Barnes-Hut par 1 workers'))

    plt.show()

if __name__ == '__main__':
    main()

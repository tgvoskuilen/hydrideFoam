import os
import csv

htx_folders = os.listdir('sets/htx')
htx_folders = sorted(htx_folders, key=lambda t:int(t))
htx_times = [float(t) for t in htx_folders]
htx_Ts = []

hyd_folders = os.listdir('sets/hydride')
hyd_folders = sorted(hyd_folders, key=lambda t:int(t))
hyd_times = [float(t) for t in hyd_folders]
hyd_Ts = []

for f in htx_folders:
    filepath = os.path.join('sets','htx',f,'cloudSS_T.csv')
    with open(filepath,'r') as csvfile:
        reader = csv.reader(csvfile)
        Ts = []
        for line in reader:
            lineend = line.pop()
            try:
                Ts.append(float(lineend))
            except ValueError:
                pass
    htx_Ts.append(Ts)
    
for f in hyd_folders:
    filepath = os.path.join('sets','hydride',f,'cloudBed_Tfluid.csv')
    with open(filepath,'r') as csvfile:
        reader = csv.reader(csvfile)
        Ts = []
        for line in reader:
            lineend = line.pop()
            try:
                Ts.append(float(lineend))
            except ValueError:
                pass
    hyd_Ts.append(Ts)
    
with open('Tpoints.csv','w') as outfile:
    writer = csv.writer(outfile,delimiter=',')
    writer.writerow(['Time (s)', 'T1', 'T2', 'T3', 'T4', 'T5'])
    
    for i,t in enumerate(htx_times):
        if t in hyd_times:
            ihy = hyd_times.index(t)
            writer.writerow([t, htx_Ts[i][0], htx_Ts[i][1], \
                             hyd_Ts[ihy][0], hyd_Ts[ihy][1], hyd_Ts[ihy][2]])
            
            

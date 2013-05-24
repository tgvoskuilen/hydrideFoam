import os
import csv

try: paraview.simple
except: from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()

TPSHolderGambit_foam = GetActiveSource()

probePoints = [[0.0, 0.0, 0.01],  \
               [0.0, 0.0, 0.015], \
               [0.0, 0.0, 0.02],  \
               [0.0, 0.0, 0.025], \
               [0.0, 0.0, 0.03],  \
               [0.0, 0.0, 0.035]]

ProbeLocations = []

for p in probePoints:
    SetActiveSource(TPSHolderGambit_foam)
    NewLocation = ProbeLocation( ProbeType="Fixed Radius Point Source" )
    NewLocation.ProbeType.Center = p
    NewLocation.ProbeType = "Fixed Radius Point Source"
    active_objects.source.SMProxy.InvokeEvent('UserEvent', 'ShowWidget')
    NewLocation.ProbeType.NumberOfPoints = 1
    NewLocation.ProbeType.Radius = 0
    ProbeLocations.append(NewLocation)

    RenderView1 = GetRenderView()
    DataRepresentation = GetDisplayProperties(TPSHolderGambit_foam)
    DataRepresentation = Show()
    DataRepresentation.EdgeColor = [0.0, 0.0, 0.5000076295109483]
    DataRepresentation.SelectionPointFieldDataArrayName = 'pFluid'
    DataRepresentation.SelectionCellFieldDataArrayName = 'pFluid'
    DataRepresentation.ColorArrayName = 'pFluid'
    DataRepresentation.ScaleFactor = 0.00017792810685932637
    a1_p_PVLookupTable = GetLookupTableForArray( "pFluid", 1 )
    DataRepresentation.LookupTable = a1_p_PVLookupTable
    Render()


# Get the times
source = FindSource("TPS.foam")
times = source.TimestepValues
F = []
T = []
view = GetActiveView()

# Extract all point data
for n,time in enumerate(times):
    if n > 0:
        print "Capturing time %d of %d" % (n+1, len(times))
        view.ViewTime = time
        Render()
        Fs = []
        Ts = []
        for PL in ProbeLocations:
            Frange = PL.PointData.GetArray("F").GetRange()
            Fmean = 0.5*(Frange[0]+Frange[1])
            Fs.append(Fmean)
            Trange = PL.PointData.GetArray("Tfluid").GetRange()
            Tmean = 0.5*(Trange[0]+Trange[1])
            Ts.append(Tmean)

        F.append(Fs)
        T.append(Ts)

print T

# Write point data to a csv file
with open('pointsF.csv','w') as csvfile:
    pointwriter = csv.writer(csvfile)
    headerrow = ["Time"]
    for p in probePoints:
        headerrow.append("Point (%f %f %f)" % (p[0], p[1], p[2]))

    pointwriter.writerow(headerrow)

    for i,t in enumerate(times):
        if i > 0:
            row = [t]
            for Fi in F[i-1]:
                row.append(Fi)
            pointwriter.writerow(row)

# Write point data to a csv file
with open('pointsT.csv','w') as csvfile:
    pointwriter = csv.writer(csvfile)
    headerrow = ["Time"]
    for p in probePoints:
        headerrow.append("Point (%f %f %f)" % (p[0], p[1], p[2]))

    pointwriter.writerow(headerrow)

    for i,t in enumerate(times):
        if i > 0:
            row = [t]
            for Ti in T[i-1]:
                row.append(Ti)
            pointwriter.writerow(row)


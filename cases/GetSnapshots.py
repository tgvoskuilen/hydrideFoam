try: paraview.simple
except: from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()

RenderView1 = GetRenderView()
RenderView1.CameraViewUp = [0.11472512944823393, 0.432504831801169, 0.8943029213536813]
RenderView1.CameraPosition = [-0.0263372257805983, -0.07243305396641156, 0.060049695448528805]
RenderView1.CameraClippingRange = [0.05996032547346617, 0.12685033879484398]
RenderView1.Background = [1.0, 1.0, 1.0]
RenderView1.Background2 = [0.0, 0.0, 0.16470588235294117]

Clip1 = GetActiveSource()
AnnotateTimeFilter1 = AnnotateTimeFilter()

TPSHolderGambit_foam = FindSource("TPS.foam")
DataRepresentation1 = GetDisplayProperties(TPSHolderGambit_foam)
DataRepresentation2 = GetDisplayProperties(Clip1)
DataRepresentation3 = Show()

AnnotateTimeFilter1.Format = 'Time: %4.0f s'

DataRepresentation3.Color = [0.0, 0.0, 0.0]
DataRepresentation3.Position = [0.4, 0.1]

WriteAnimation('U:/Personal/animations/case3_T.png', Magnification=1, Quality=2, FrameRate=1.000000)


Render()
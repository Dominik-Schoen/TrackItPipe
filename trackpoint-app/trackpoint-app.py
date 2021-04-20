#!/usr/bin/env python3

import numpy
import stl
from stl import mesh
import pygmsh

with pygmsh.geo.Geometry() as geom:
    circle = geom.add_circle([0.0, 0.0], 1.0, mesh_size=0.2)
    #geom.extrude(circle, [0.0, 0.3, 1.0], num_layers=5)
    cube = geom.generate_mesh()

meshx = mesh.Mesh.from_file("../testdata/zPlate_0.stl")



combi = mesh.Mesh(numpy.concatenate([
    meshx.data.copy(),
    cube.data.copy(),
]))

combi.save("combi.stl", mode=stl.Mode.ASCII)
import bpy
import math
import struct

obj = bpy.context.active_object.data
vertices = obj.vertices

f = open("./voxels.bin", "wb");

i = 0
idx = set()
for poly in obj.polygons:
    for id in poly.loop_indices:
        loop = obj.loops[id]
        v = loop.vertex_index
        
        if v not in idx:
            idx.add(v)
            x = math.floor(vertices[v].co.x)
            y = math.floor(vertices[v].co.y)
            z = math.floor(vertices[v].co.z)
            
            c = obj.vertex_colors.active.data[i].color
            
            r = math.floor(c.r*255)
            g = math.floor(c.g*255)
            b = math.floor(c.b*255)
        
            color = (r << 24) + (g << 16) + (b << 8) + 255
            
            f.write(struct.pack("<I", color))
            f.write(struct.pack("<h", int(x)))
            f.write(struct.pack("<h", int(y)))
            f.write(struct.pack("<h", int(z)))
            f.write(struct.pack("<h", 1))
        
        i += 1
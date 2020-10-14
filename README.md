# Atomic Point of View | Generator
```
Nom de code: APoV.

APoV, standing for Atomic Point of View, is a variant of voxel raytracing,
proposing an alternative to its realtime calculation cost. The main idea is
to record the information for a ray, on different angles and from the point of
view of each voxel, in space. This will produce huge amount of raw data,
representing the 3d space in multiple point of views. The data could be then
read or streamed, bringing the advantage of a fast rendering result.

The current generator records only 2 informations by ray, per space voxel. The
RGB color of the scanned voxel, and the ray depth which is the length between
the space voxel and the scanned voxel. The depth information could be used by
the navigator to produce realtime effect.


First, export the voxelized object via blender using the available script.

Then, generate the atomic raw data with the following for a locked camera:
./bin/apov space-block-size:256 atomic-pov-count:180 ray-step:2 \
    max-ray-depth:256 projection-depth:300

Or, for a free camera:
./bin/apov space-block-size:256 atomic-pov-count:180 ray-step:2 \
    max-ray-depth:256 projection-depth:300 cam-lock-ahead


For PSP generate the raw data with:
./bin/apov space-block-size:256 atomic-pov-count:90 ray-step:4 \
    max-ray-depth:256 projection-depth:300
    
### Available options
space-block-size: ......... Minimun size of a region block default is 256^3
atomic-pov-count: ......... Step angle between two point of views
ray-step: ................. 
max-ray-depth: ............ The max length of the ray during the raytracing
width-block-count: ........ Number of blocks in width
depth-block-count: ........ Number of blocks in depth
projection-depth: ......... Value used to pre-render with projection
projection-gaps-reducer: .. 
cam-distance: ............. 
cam-lock-at: .............. 
cam-hemisphere ............ 
anti-aliasing ............. Pre-render with anti-aliasing filter on
cam-lock-ahead ............ 


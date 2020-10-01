# APoV-generator
```
APoV, standing for Atomic Point of View, is a variant of raytracing, proposing
an alternative to it's realtime calculation cost. The main idea is to record
the information of a ray, on different angles and from the point of view of each
voxel, in space. This will produce huge amount of raw data, representing the 3d
space in multiple point of views. The data could be then read or streamed,
bringing the advantage of a fast rendering result.

The current generator records only 2 informations by ray, per space voxel. The
RGB color of the scanned voxel, and the ray depth which is the length between
the space voxel and the scanned voxel. The depth information will be used by
the navigator to produce the perspective effect.

First, export the voxelized object via blender using the available script.

Then, generate the atomic raw data with the following for a locked camera:
./bin/apov space-size:256 atomic-pov-count:180 `
    ray-step:2 max-ray-depth:128 cam-locked

Or, for a free camera:
./bin/apov space-size:256 atomic-pov-count:180 `
    ray-step:2 max-ray-depth:128 cam-hemisphere
# Atomic Point of View | Generator
```
### Nom de code: APoV
APoV, standing for Atomic Point of View, is a variant of voxel raytracing,
proposing an alternative to its realtime calculation cost. The main idea is
to record the information for a ray, in a given 3d space region, on different
angles and from the point of view of each voxel. This will produce a huge amount
of raw data, representing the region in multiple point of views. The data could
then be read or streamed, bringing the advantage of a fast rendering result.

The current generator exports two informations by ray, per space voxel. The
RGB color of the scanned voxel (can be blended), and the ray depth which is
the length between the space voxel and the scanned voxel. The depth information
could then be used by an APoV Navigator to visualize the volume and/or to produce
realtime effects.


### The voxels.bin file entry
To generate the APoV data file, you need a voxels file entry. You can use one
of the available voxels.bin file from the zip or generate your own via Blender.
The apov-tga-raw-converter is recommended to get the voxels.bin from any 3d model.
In this project you will find a scanner working on Blender 2.9+. Just place your
mesh at the center of the scene and render the animation. This will generate tga
files which can be converted into a voxels.bin file entry with the use of the
apov-tga-raw-converter tool.

Alternatively you can use the available python script with blender 2.78 to get
the APoV file entry. First set the grid unit size to 1, remesh your object into
blocks, ajust the scale value of the modifier to get the blocks aligned with the
units of the grid. Apply the transformations, set the object position at 0.5,
0.5, 0.5. Apply position, then export the object using the python script.


### Generate the streamable data
Copy the voxels.bin at the root of the repository. Then, as an example, run the
following command to produce an APoV region with a locked camera navigation:
./bin/apov space-block-size:256 vertical-pov-count:36 horizontal-pov-count:36 \
    ray-step:16 max-ray-depth:192 projection-depth:300

Or, for a free camera navigation:
./bin/apov space-block-size:256 vertical-pov-count:1 horizontal-pov-count:90 \
    ray-step:2 max-ray-depth:192 projection-depth:300 cam-lock-ahead


For PSP, to get a smaller raw file, you can run something like the following:
./bin/apov space-block-size:256 vertical-pov-count:1 horizontal-pov-count:72 \
    ray-step:8 max-ray-depth:192 projection-depth:300

Or by using indexed colors.


### Indexed colors & CLUT
Below, an example of how to generate an indexed image with its corresponding
color lookup table. All available modes are listed in the options section.

./bin/apov space-block-size:256 vertical-pov-count:45 horizontal-pov-count:45 \
    ray-step:8 max-ray-depth:192 projection-depth:300 export-clut compress-clut \
    clut-compression-mode:luminance

An other example rescaling the scanned region and using alpha blending.
./bin/apov space-block-size:256 vertical-pov-count:45 horizontal-pov-count:45 \
    ray-step:16 max-ray-depth:384 max-blend-depth:384 enable-blending \
    enable-deep-transparency projection-depth:400 export-clut compress-clut \
    clut-compression-mode:rgb clut-compression-factor:6 scale:0.9
    
Depending on the number of color you might need to ajust the CLUT compression by
using the clut-compression-factor option.


### 1 Bit Color Mapping
1 bit color mapping or 1BCM is about generating two frames to be mapped for
drawing the current point of view. The first frame is produced by recording each
visible voxel as a bit. The other is a rendering of the visible colored voxels
in very low definition. This allows the APoV generator to significantly reduce
the amount of its output data. The depth information is still exported, but will
be associated with a group of voxels instead of a single one.

As an example, this is how to generate a streamable region of 45x45 point of
views, using 1 Bit Color Mapping with a RGB map of 8x8 pixels:
./bin/apov space-block-size:256 vertical-pov-count:45 horizontal-pov-count:45 \
    ray-step:256 max-ray-depth:128 projection-depth:400 use-1bit-color-mapping \
    export-header color-map-size:8


### Available options
____________________________
space-block-size: ......... Minimun size of a region block (default is 256^3)
width-block-count: ........ Number of blocks in width
depth-block-count: ........ Number of blocks in depth
____________________________
horizontal-pov-count: ..... Step angle between two horizontal point of views
vertical-pov-count: ....... Step angle between two vertical point of views
ray-step: ................. Change the value of the ray step frame generation
max-ray-depth: ............ The max length of the ray during the raytracing
____________________________
cam-distance: ............. Sets a distance between the camera and the region
cam-lock-at: .............. Lock the camera at a given depth position
cam-hemisphere ............ Pre-render only the front hemisphere of the camera
cam-lock-ahead ............ Lock the camera at the beginning of the space region
____________________________
projection-depth: ......... Value used to pre-render the region with projection
____________________________
enable-anti-aliasing ...... Pre-render with the anti-aliasing filter on
anti-aliasing-threshold: .. Threshold between the edges and the surrounding
blur-smooth-matrix: ....... 
blur-mode: ................ 
scale: .................... Define a new scale for the rendered content.
____________________________
export-clut ............... Exports a CLUT + Indexes instead of raw rgb data
compress-clut ............. Try to turn the CLUT into a CLUT of 255 colors only
clut-compression-mode: .... Available modes are ycbcr, rgb, luminance & average
clut-compression-factor: .. Need to be used if the clut is saturated
____________________________
export-header ............. Exports an header of 80 bytes containing the main
........................... options used to generate the file
____________________________
enable-blending ...........
max-blend-depth ...........
enable-deep-transparency ..
____________________________
use-1bit-color-mapping .... Exports each data as a bit, which can be mapped with
........................... the corresponding 2d color map. 
color-map-size: ........... Defines the size of the color map.
enable-trace-edges ........
trace-edges-threshold .....


# Simple Collage

Simple Collage is a simple app for combining and transforming multiple images. 
It's not an editor. It acts more like an advanced image viewer.

## General usage

- Add image files to the collage using "File - Add image..." in menu or drag & drop. 
- Move added images using left mouse button.
- Select images using left mouse button. Select multiple images with Ctrl. Clear selection by clicking on empty space.
- Rotate, zoom, and reflect selected images (or full view if no selection) using toolbar buttons.
- Press "Reset" to reset default zoom, rotation, and reflection of selected images (or full view if no selection).
- To crop an image, select it, press "Crop" button, adjust the frame, and press "Apply".
- To undo cropping, select the image, press "Crop" button, and press "Cancel".
- Zoom the view at mouse cursor using Ctrl + mouse scroll.
- Press "Save" to save the collage in a file.
- Last used collage file is automatically opened at program startup.

## Collage file

Collage file (*.scl) contains information about included images and their transformations. 
Paths to images are stored relatively to the collage file. Original images are not modified.

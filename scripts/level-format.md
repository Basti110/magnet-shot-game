# GameDev Level Format (JSON)

General info:

* Names are unique (blender requirement)
* Blender supports custom properties (name-double pairs) for: objects, lights, materials, world
* All vectors and matrices are in blender coordinates (z is up)
* The importer will fix all vectors and matrices on-the-fly

## Structure

### Top-level Structure
```
{
    "objects": [ ... ],
    "lights": [ ... ],
    "materials": [ ... ],
    "global": global-parameters
}
```

### Objects

* Objects use euler angles because they are easy to export from blender.
In the engine, these angles are converted to a 3x3 matrix.
* Properties values are number-only.
* Colors are blender per-object-colors
* To add objects to physics, click `Physics->Rigid Body` in Blender. (Active = False will make objects static)

```
{
    "name": "<OBJECT NAME>",
    "position": [x, y, z],
    "frame": [[m00, m01, m02], [m10, m11, m12], [m20, m21, m22]], // local 3x3 coordinate frame, contains rotation and scale
    "color": [r, g, b, a], // object color includes alpha
    "materials": [
        "mat-name-0",
        "mat-name-1",
        ...
    ],
    "filepath": "<RELATIVE PATH TO OBJ>",
    (optional) "physics": { // no physics entry means no physics representation
        "mass": 1.0,        // mass in kg
        "friction": 0.5,    // resistance of object to movement
        "bounciness": 0.0,  // technically called restitution
        "shape": "SPHERE",  // collision shape, see blender for options
        "dynamic": true,    // if false, object is static
        "linearDamping": 0.04, // velocity damping
        "angularDamping": 0.1, // angular velocity damping
    }
    "properties": {
        "name0": value0,
        "name1": value1,
        ...
    }
}
```

### Lights

```
{
    "name": "<LIGHT NAME>",
    "color": [r, g, b],
    "type": "POINT", // types: POINT, SUN
    "position": [x, y, z], // for POINT
    "direction": [x, y, z], // for SUN
    "properties": {
        "name0": value0,
        "name1": value1,
        ...
    }
}
```

### Materials

* Each object can use multiple materials
* Each material can be used by multiple objects

```
{
    "name": "<MATERIAL NAME>",
    "alpha": 1.0,    // transparent 0..1 opaque
    "ambient": 1.0,  // amount of global ambient color received
    "diffuse": [r, g, b],
    "specular": [r, g, b],
    "specularHardness": 50.0, // specular exponent basically
    "useObjectColor": false, // if true, color is multiplied by object color
    "textures": {
        "usage-name": { // name like 'diffuse' or 'normal'
            "path": "relative/path/to/texture",
            "colorspace": "sRGB", // or 'Linear'
            "offset": [ox, oy],
            "scale": [sx, sy], // tex coords = offset + uv * scale
        },
        ...
    },
    "properties": {
        "name0": value0,
        "name1": value1,
        ...
    }
}
```

### Global parameters

* Global scene parameters
* Correspond to blender "World" parameters

```
{
    "ambient": [r, g, b],  // global ambient color
    "properties": {
        "name0": value0,
        "name1": value1,
        ...
    }
}
```

#!/usr/bin/env python
import bpy
import json
import os

# convenience imports
from mathutils import *
from math import *

# convenience variables
C = bpy.context
D = bpy.data


# Returns the json for custom properties of a given object
def custom_properties(obj):
    props = {}
    for k in obj.keys():
        if isinstance(obj[k], float):
            props[k] = obj[k]
    return props


# void main()
if __name__ == "__main__":
    print("")
    print("==== GameDev Python Exporter ====")
    print("")

    # ============================================
    # Sanity checks

    # Check if .blend is loaded
    if bpy.data.filepath == "":
        print("Filename is empty. Please load a .blend file first!")
        sys.exit(1)

    # Only one scene and one world
    if len(D.worlds) > 1:
        print("Only one world allowed")
        sys.exit(1)
    if len(D.scenes) > 1:
        print("Only one scene allowed")
        sys.exit(1)

    # ============================================
    # Populate scene
    level = {}
    world = D.worlds[0]
    scene = D.scenes[0]

    # ============================================
    # Objects
    print("Objects:")
    objs = []
    for o in D.objects:
        if o.type != 'MESH':
            continue

        print("  " + o.name)
        path = bpy.data.filepath + ".data/meshes/" + o.name + ".obj"
        rpath = os.path.basename(bpy.data.filepath) + ".data/meshes/" + o.name + ".obj"
        jobj = {
            'name': o.name,
            'position': list(o.location),
            'frame': list(list(x) for x in o.matrix_world.to_3x3()),
            'color': list(o.color),
            'materials': list(m.name for m in o.material_slots),
            'filepath': rpath,
            'properties': custom_properties(o),
        }

        # physics
        if o.rigid_body is not None:
            r = o.rigid_body
            jobj['physics'] = {
                'mass': r.mass,
                'dynamic': r.enabled,
                'friction': r.friction,
                'bounciness': r.restitution,
                'shape': r.collision_shape,
                'linearDamping': r.linear_damping,
                'angularDamping': r.angular_damping,
            }

        # save mesh
        mesh = o.to_mesh(scene, True, 'RENDER')
        # deselect all
        bpy.ops.object.select_all(action='DESELECT')
        # select only this
        o.select = True
        # set world matrix to identity
        mw = o.matrix_world.copy()
        o.matrix_world.identity()
        # save to obj
        os.makedirs(bpy.data.filepath + ".data/meshes", exist_ok=True)
        bpy.ops.export_scene.obj(filepath=path,
                                 check_existing=False,  # don't warn on override
                                 use_selection=True,    # use selected
                                 use_edges=False,       # don't include edges
                                 use_materials=False,   # don't generate material files
                                 use_normals=True,      # save normals
                                 use_triangles=True,    # triangulate faces
                                 path_mode='ABSOLUTE',  # use absolute pathes
                                 )
        print("    (wrote " + str(len(mesh.vertices)) + " vertices and " + str(len(mesh.polygons)) + " polygons to " + rpath + ")")
        # restore orientation
        o.matrix_world = mw

        objs.append(jobj)
    print("")
    level['objects'] = objs

    # ============================================
    # Lights
    print("Lights:")
    lights = []
    for l in D.objects:
        if l.type != 'LAMP':
            continue

        print("  " + l.name)
        jlight = {
            'name': l.name,
            'color': list(l.data.color),
            'type': l.data.type,
            'properties': custom_properties(l),
        }

        if l.data.type == "POINT":
            jlight['position'] = list(l.location)
        elif l.data.type == "SUN":
            mw = l.matrix_world
            jlight['direction'] = [mw[0][2], mw[1][2], mw[2][2]]
        else:
            print("WARNING: Light " + l.name + " ignored: type " + l.data.type + " not supported")
            continue

        lights.append(jlight)
    print("")
    level['lights'] = lights

    # ============================================
    # Materials
    print("Materials:")
    mats = []
    for m in D.materials:
        print("  " + m.name)
        jmat = {
            'name': m.name,
            'alpha': m.alpha,
            'diffuse': list(m.diffuse_color),
            'specular': list(m.specular_color),
            'specularHardness': float(m.specular_hardness),
            'useObjectColor': m.use_object_color,
            'properties': custom_properties(m),
        }

        # Textures
        textures = {}
        for t in m.texture_slots:
            if t is None:
                continue  # slot empty

            if t.texture is None:
                continue  # no texture attached

            if t.texture.type != "IMAGE":
                print("WARNING: Texture " + t.name + " of material " + m.name + " ignored: not an IMAGE")
                continue

            if t.texture_coords != "UV":
                print("WARNING: Texture " + t.name + " of material " + m.name + " ignored: only UV coordinate mapping supported")
                continue

            jtex = {
                'path': t.texture.image.filepath,
                'scale': t.scale[0:2],
                'offset': t.offset[0:2],
                'colorspace': t.texture.image.colorspace_settings.name
            }

            # texture usage
            usage = None
            if t.use_map_color_diffuse:
                usage = "diffuse"
            elif t.use_map_normal:
                usage = "normal"
            else:
                print("WARNING: Texture " + t.name + " of material " + m.name + " ignored: no known purpose (like diffuse or normal map)")
                continue

            # write to json
            textures[usage] = jtex
        jmat["textures"] = textures

        mats.append(jmat)
    print("")
    level['materials'] = mats

    # ============================================
    # Global
    gpara = {
        'ambient': list(world.ambient_color[:]),
        'properties': custom_properties(world),
    }
    level['global'] = gpara

    # ============================================
    # Save file
    jsonFile = bpy.data.filepath + ".json"
    with open(jsonFile, 'w') as outfile:
        json.dump(level, outfile, indent=4)
    print("Wrote JSON to " + jsonFile)
    
    print("")
    print("=================================")
    print("")
    quit()

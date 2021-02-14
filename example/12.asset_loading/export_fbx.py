import bpy
import sys
import getopt

print("Blender export scene in FBX Format in file "+sys.argv[-1])

# Doc can be found here: https://docs.blender.org/api/current/bpy.ops.export_scene.html
bpy.ops.export_scene.fbx(
	filepath=sys.argv[-1],
	embed_textures=False,
	global_scale=.01,
	#apply_unit_scale=True,
	#bake_space_transform=True,
	axis_forward='-Z',
	axis_up='Y'
)

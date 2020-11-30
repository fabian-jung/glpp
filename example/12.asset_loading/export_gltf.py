import bpy
import sys
import getopt

print("Blender export scene in GLB Format in file "+sys.argv[-1])

# Doc can be found here: https://docs.blender.org/api/current/bpy.ops.export_scene.html
bpy.ops.export_scene.gltf(
	filepath=sys.argv[-1], 
	export_format='GLB',
	export_tangents=False,
	export_materials='EXPORT',
	export_cameras=True,
	export_yup=True,
	export_apply=True,
	export_lights=True,
	will_save_settings=False,
	check_existing=False
)

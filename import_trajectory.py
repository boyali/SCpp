import numpy as np
import os
import random
import bpy

# set current path
abspath = os.path.abspath(__file__)
dname = os.path.dirname(os.path.dirname(abspath))
os.chdir(dname)

scn = bpy.context.scene

# maximum thrust for scaling
thrust_magnitude = 20
FPS = scn.render.fps

# set output folder and get highest index
data_folder = 'output/Rocket3D/'
data_folder += sorted(os.listdir(data_folder))[-1]

# load data
X = np.loadtxt(open(f"{data_folder}/X_full.txt", "rb"))
U = np.loadtxt(open(f"{data_folder}/U_full.txt", "rb"))
# t = np.loadtxt(open(f"{data_folder}/t.txt", "rb"))
t = 24.

# get objects
rck = bpy.data.objects["rck"]
eng = bpy.data.objects["eng"]

# get timesteps, set total frames and timestep
K = X.shape[1]
trajectory_frames = FPS * t
step_size = trajectory_frames / K

# clear animation data
rck.animation_data_clear()
eng.animation_data_clear()

current_frame = 1
scn.frame_current = 1
# for each timestep in trajectory
for i in range(K):
    current_frame += step_size
    scn.frame_current = int(current_frame)

    x = X[:, i]
    u = U[:, i]

    # location and rotation
    rck.location = np.array([x[0], x[1], x[2]]) * 10
    q = np.array((x[6], x[7], x[8]))
    rck.rotation_quaternion = [np.sqrt(1. - x[6:9].dot(x[6:9])), x[6], x[7], x[8]]
    print(rck.rotation_quaternion)
    rck.keyframe_insert(data_path='rotation_quaternion')
    rck.keyframe_insert(data_path='location')

    # thrust magnitude and angles
    rx = np.arctan(-u[1] / u[2])
    ry = np.arctan(u[0] / u[2])
    eng.rotation_euler = (rx, ry, 0)

    eng.keyframe_insert(data_path='rotation_euler')

# shutdown engine

scn.frame_current += FPS

# set frame range
scn.frame_start = 1
scn.frame_end = scn.frame_current
# go back to start
scn.frame_current = 1

# # select all objects
# bpy.context.area.type = 'VIEW_3D'
# bpy.ops.object.select_all(action='SELECT')

# # set all to linear interpolation
# bpy.context.area.type = 'GRAPH_EDITOR'
# bpy.ops.graph.select_all(action='SELECT')
# bpy.ops.graph.interpolation_type(type='LINEAR')

# # deselect all objects
# bpy.context.area.type = 'VIEW_3D'
# bpy.ops.object.select_all(action='DESELECT')

# bpy.context.area.type = 'TEXT_EDITOR'

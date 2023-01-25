import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

first_status_filepath  = "Progetto-AMSC-NBody/build/Progetto AMSC NBody/particles_initial_output.pt"
filepath = "Progetto-AMSC-NBody/build/Progetto AMSC NBody/particles_output.pt"

def visualize_file(filepath):
    ints = np.fromfile(filepath, dtype=int)
    x = np.fromfile(filepath, dtype=float)

    dim = ints[0]
    number_of_particles = int(ints[1])
    #print(ints)


    print("Problem of " , number_of_particles, "bodies in ", dim , "D")

    data = x[1:]
    # I have to break data into number_of_particles chunks
    new_len = int(data.shape[0] / number_of_particles)
    new_shape = (number_of_particles, new_len)
    data = data.reshape(new_shape)

    #print(data)

    # now i have an array, data, in which every row is : pos[0], pos[1], pos[2], speed[0], speed[1], speed[2], accel[0], accel[1], accel[2], mass, color
    # i have to plot it
    fig = plt.figure(figsize=(4,4))
    ax = fig.add_subplot(111, projection='3d')
    for index in range (0,number_of_particles):
        new_color = '#'+'{0:06X}'.format(int(data[index][-1]))
        ax.scatter(data[index][0], data[index][1],data[index][2], color = new_color)

    plt.show()

visualize_file(filepath=first_status_filepath)
visualize_file(filepath=filepath)
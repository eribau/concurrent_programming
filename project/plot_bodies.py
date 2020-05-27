import numpy as np
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.animation as animation

length = 1
Writer = animation.writers['ffmpeg']
writer = Writer(fps=120, metadata=dict(artist='Me'), bitrate=1800)

def get_data():
    data = pd.read_csv("data/positions.csv", header=None)
    x = np.array(data.loc[:, ::2])
    y = np.array(data.loc[:, 1::2])
    return (x, y)

fig = plt.figure()
ax = plt.axes(xlim=(0, length), ylim=(0, length))
bodies = ax.scatter(x=[], y=[])
x, y = get_data()


def init():
    return bodies,

def animate(i):
    # print(np.c_[x[0], y[0]].shape)
    bodies.set_offsets(np.c_[x[i], y[i]])
    return bodies,

ani = animation.FuncAnimation(fig, animate, frames=len(x),
                                interval=10, blit=True, init_func=init)

# plt.show()

ani.save('data/animation.mp4', writer=writer)

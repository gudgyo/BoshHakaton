import serial
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Initialize Arduino connection
arduino = serial.Serial(port='/dev/cu.usbserial-0001', baudrate=115200, timeout=0.1)

# Initialize variables for plotting
time_window = 10  # Time window in seconds
max_data_points = 200  # Maximum number of data points to display
x_data = np.linspace(-time_window, 0, max_data_points)  # X-axis data (time)
y_data = np.zeros((4, max_data_points))  # Y-axis data (sensor values)

# Create subplots for a 2x2 grid
fig, axs = plt.subplots(2, 2, facecolor='#2E2E2E')  # Darker shade of gray

# Colors for lines
colors = ['blue', 'green', 'red', 'purple']

# Initialize line objects for each subplot
lines = [ax.plot(x_data, y_data[i], color=colors[i])[0] for i, ax in enumerate(axs.flat)]

battery = 100

# Add labels and titles to each subplot with white color
labels = ['Trigger pulled (%)', 'Rotation per minute', 'Battery (%)', 'Torque (Nm)']
for i, ax in enumerate(axs.flat):
    ax.set_xlabel('Past 10 seconds', color='white')
    ax.set_ylabel(labels[i], color='white')
    ax.set_title(labels[i], color='white')

    # Set background color for the subplot
    ax.set_facecolor('#1F1F1F')  # Slightly darker shade of gray

    # Set grid color
    ax.grid(color='gray', linestyle='--', alpha=0.5)

    # Set spine color
    for spine in ax.spines.values():
        spine.set_edgecolor('white')

# Adjust spacing between subplots
plt.subplots_adjust(left=0.1, right=0.9, bottom=0.1, top=0.9, wspace=0.4, hspace=0.4)


def plot_past_10_seconds(new_entry, figure_index):
    global y_data

    # Append the new entry to the y_data array for the specified figure
    y_data[figure_index] = np.roll(y_data[figure_index], -1)
    y_data[figure_index, -1] = new_entry

    # Update the plot data for the specified figure
    lines[figure_index].set_ydata(y_data[figure_index])


def write_read():
    data = arduino.readline().decode('utf-8')
    global battery
    if len(data) > 0:
        values = data.split(',')
        trigger = int(values[0]) / 4095 * 100
        if battery >= 0:
            battery -= trigger / 1000
        else:
            values[1] = 0
            values[3] = 0
        plot_past_10_seconds(trigger, 0)
        plot_past_10_seconds(trigger * 6.2, 1)
        plot_past_10_seconds(battery, 2)
        plot_past_10_seconds(trigger / 9, 3)



def update(frame):
    write_read()
    for i, ax in enumerate(axs.flat):
        ax.relim()
        ax.autoscale_view(tight=True)
    return lines


# Set up the animation
ani = FuncAnimation(fig, update, interval=50)

plt.show()

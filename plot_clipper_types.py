import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
data = pd.read_csv('clipper_results_by_type.csv')

# Create the plot
plt.figure(figsize=(12, 8))

# Plot each clipper type with a different color
clipper_types = ['Atan', 'Cubic', 'Hard', 'Smooth', 'Tanh']
colors = ['blue', 'green', 'red', 'purple', 'orange']

# Plot the reference line (y=x) to show the clipping effect
plt.plot(data['Input'], data['Input'], 'k--', alpha=0.5, label='y=x (no clipping)')

# Plot each clipper type
for i, clipper_type in enumerate(clipper_types):
    plt.plot(data['Input'], data[clipper_type], color=colors[i], linewidth=2, label=clipper_type)

# Add labels and title
plt.xlabel('Input')
plt.ylabel('Output')
plt.title('Comparison of Different Clipper Types')
plt.grid(True)
plt.legend()

# Add horizontal lines at y=-1 and y=1 to show common clipping thresholds
plt.axhline(y=1, color='gray', linestyle=':', alpha=0.7)
plt.axhline(y=-1, color='gray', linestyle=':', alpha=0.7)

# Save the plot
plt.savefig('clipper_types_comparison.png')

# Show the plot
plt.show()
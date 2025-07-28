#!/usr/bin/env python3
import os
import glob
import pandas as pd
import matplotlib.pyplot as plt

def plot_csv_file(file_path):
    """
    Plot a CSV file as a line graph and save the plot.
    
    Args:
        file_path: Path to the CSV file
    """
    # Extract the filename without extension for the plot title and output file
    filename = os.path.basename(file_path)
    name_without_ext = os.path.splitext(filename)[0]
    
    # Read the CSV file
    df = pd.read_csv(file_path)
    
    # Create a figure with a reasonable size
    plt.figure(figsize=(10, 6))
    
    # Plot each column as a separate line
    for column in df.columns:
        plt.plot(df[column], label=column)
    
    # Add labels and title
    plt.title(f"Plot of {name_without_ext}")
    plt.xlabel("Sample Index")
    plt.ylabel("Value")
    plt.legend()
    plt.grid(True)
    
    # Save the plot
    output_path = os.path.join(os.path.dirname(file_path), f"../images/{name_without_ext}_plot.png")
    plt.savefig(output_path)
    plt.close()
    
    print(f"Plot saved to {output_path}")

def main():
    # Get the directory of this script
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # get parent path of script directory
    parent_dir = os.path.dirname(script_dir)
    
    # Path to the csv directory
    csv_dir = os.path.join(parent_dir, "csv")
    
    # Find all NTM_* CSV files
    ntm_files = glob.glob(os.path.join(csv_dir, "NTM_*.csv"))
    
    if not ntm_files:
        print("No NTM_* CSV files found in the csv directory.")
        return
    
    print(f"Found {len(ntm_files)} NTM_* CSV files.")
    
    # Plot each file
    for file_path in ntm_files:
        print(f"Processing {os.path.basename(file_path)}...")
        plot_csv_file(file_path)

if __name__ == "__main__":
    main()
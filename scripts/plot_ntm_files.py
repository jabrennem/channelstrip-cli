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
    
    # Plot input and output if they exist, otherwise plot all columns
    if 'input' in df.columns and 'output' in df.columns:
        plt.plot(df['input'], label='Input', alpha=0.7)
        plt.plot(df['output'], label='Output', alpha=0.7)
    else:
        for column in df.columns:
            if column != 'sample':
                plt.plot(df[column], label=column)
    
    # Add labels and title
    plt.title(f"Plot of {name_without_ext}")
    plt.xlabel("Sample Index")
    plt.ylabel("Amplitude")
    plt.legend()
    plt.grid(True)
    
    # Save the plot
    output_path = os.path.join(os.path.dirname(file_path), f"../images/{name_without_ext}_plot.png")
    plt.savefig(output_path)
    plt.close()
    
    print(f"Plot saved to {output_path}")

def main():
    import sys
    
    if len(sys.argv) != 2:
        print("Usage: python plot_ntm_files.py <csv_file_path>")
        return
    
    file_path = sys.argv[1]
    
    if not os.path.exists(file_path):
        print(f"File not found: {file_path}")
        return
    
    print(f"Processing {os.path.basename(file_path)}...")
    plot_csv_file(file_path)

if __name__ == "__main__":
    main()
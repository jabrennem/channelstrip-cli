#!/usr/bin/env python3
import os
import glob
import pandas as pd
import matplotlib.pyplot as plt

def plot_csv_file(file_path, max_samples=None):
    """
    Plot a CSV file as a line graph and save the plot.
    
    Args:
        file_path: Path to the CSV file
        max_samples: Maximum number of samples to display (None for all)
    """
    # Extract the filename without extension for the plot title and output file
    filename = os.path.basename(file_path)
    name_without_ext = os.path.splitext(filename)[0]
    
    # Read the CSV file
    df = pd.read_csv(file_path)
    
    # Limit samples if max_samples is specified
    if max_samples is not None and len(df) > max_samples:
        df = df.head(max_samples)
    
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
    title = f"Plot of {name_without_ext}"
    if max_samples is not None and len(df) == max_samples:
        title += f" (first {max_samples} samples)"
    plt.title(title)
    plt.xlabel("Sample Index")
    plt.ylabel("Amplitude")
    plt.legend()
    plt.grid(True)
    
    # Save the plot
    output_path = os.path.join(f"./images/{name_without_ext}_plot.png")
    plt.savefig(output_path)
    plt.close()
    
    print(f"Plot saved to {output_path}")

def main():
    import sys
    import argparse
    
    parser = argparse.ArgumentParser(description='Plot CSV files as line graphs')
    parser.add_argument('csv_file', help='Path to the CSV file')
    parser.add_argument('--max-samples', type=int, help='Maximum number of samples to display')
    
    args = parser.parse_args()
    
    if not os.path.exists(args.csv_file):
        print(f"File not found: {args.csv_file}")
        return
    
    print(f"Processing {os.path.basename(args.csv_file)}...")
    plot_csv_file(args.csv_file, args.max_samples)

if __name__ == "__main__":
    main()
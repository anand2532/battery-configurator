import tkinter as tk
from tkinter import ttk
import subprocess
import os

class BatteryConfiguratorGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("UAV Battery Configurator")
        
        # Create main frame with padding
        main_frame = ttk.Frame(root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Input Section
        ttk.Label(main_frame, text="Battery Requirements", font=('Arial', 12, 'bold')).grid(row=0, column=0, columnspan=2, pady=10)
        
        # Target Power
        ttk.Label(main_frame, text="Target Power (Wh):").grid(row=1, column=0, sticky=tk.W)
        self.target_power = ttk.Entry(main_frame)
        self.target_power.grid(row=1, column=1, padx=5, pady=2)
        
        # Cell Specifications
        ttk.Label(main_frame, text="Cell Specifications", font=('Arial', 12, 'bold')).grid(row=2, column=0, columnspan=2, pady=10)
        
        labels = ['Cell Capacity (mAh):', 'Cell Voltage (V):', 'Max Current (A):', 
                 'Cell Weight (g):', 'Length (mm):', 'Width (mm):', 'Height (mm):']
        self.cell_inputs = {}
        
        for i, label in enumerate(labels):
            ttk.Label(main_frame, text=label).grid(row=i+3, column=0, sticky=tk.W)
            self.cell_inputs[label] = ttk.Entry(main_frame)
            self.cell_inputs[label].grid(row=i+3, column=1, padx=5, pady=2)
        
        # Calculate Button
        ttk.Button(main_frame, text="Calculate Configurations", 
                  command=self.calculate_configurations).grid(row=10, column=0, 
                  columnspan=2, pady=20)
        
        # Results Section
        self.result_text = tk.Text(main_frame, height=15, width=50)
        self.result_text.grid(row=11, column=0, columnspan=2, pady=10)
        
    def calculate_configurations(self):
        # Get input values
        try:
            target_power = float(self.target_power.get())
            cell_specs = {
                'capacity': float(self.cell_inputs['Cell Capacity (mAh):'].get()),
                'voltage': float(self.cell_inputs['Cell Voltage (V):'].get()),
                'maxCurrent': float(self.cell_inputs['Max Current (A):'].get()),
                'weight': float(self.cell_inputs['Cell Weight (g):'].get()),
                'length': float(self.cell_inputs['Length (mm):'].get()),
                'width': float(self.cell_inputs['Width (mm):'].get()),
                'height': float(self.cell_inputs['Height (mm):'].get())
            }
            
            # Write cell specs to temporary file
            with open('cell_specs.txt', 'w') as f:
                f.write(f"{target_power}\n")
                for value in cell_specs.values():
                    f.write(f"{value}\n")
            
            # Call C program
            subprocess.run(['./battery_calculator', 'cell_specs.txt', 'output.txt'])
            
            # Read and display results
            self.display_results('output.txt')
            
        except ValueError:
            self.result_text.delete(1.0, tk.END)
            self.result_text.insert(tk.END, "Please enter valid numbers for all fields")
    
    def display_results(self, filename):
        try:
            with open(filename, 'r') as f:
                results = f.read()
                self.result_text.delete(1.0, tk.END)
                self.result_text.insert(tk.END, results)
        except FileNotFoundError:
            self.result_text.delete(1.0, tk.END)
            self.result_text.insert(tk.END, "Error calculating configurations")

if __name__ == "__main__":
    root = tk.Tk()
    app = BatteryConfiguratorGUI(root)
    root.mainloop()
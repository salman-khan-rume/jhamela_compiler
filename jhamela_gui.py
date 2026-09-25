#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
JHAMELA Compiler GUI
Vertical split: Input (left) | Output (right)
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, filedialog, messagebox
import subprocess
import os
import sys

class JhamelaCompilerGUI:
    def __init__(self, root, compiler_exe_path=None):
        self.root = root
        self.root.title("JHAMELA Compiler - ঝামেলা কম্পাইলার by Salman & Akash")
        self.root.geometry("1400x800")
        
        # Default compiler path
        self.compiler_path = compiler_exe_path or "jhamela.exe"
        self.temp_input_file = "temp_input.jh"
        
        # Configure style
        self.root.configure(bg="#2b2b2b")  # Soft Dark Gray Background
        style = ttk.Style()
        style.theme_use('clam')
        
        # --- FIX FOR THE "WHITE STRIPES" (SCROLLBARS) ---
        # Configure scrollbars to be dark gray instead of white
        style.configure("Vertical.TScrollbar", 
                        background="#4a4a4a",  # Dark Gray Thumb
                        troughcolor="#2b2b2b", # Matches background
                        bordercolor="#2b2b2b",
                        arrowcolor="#d4d4d4",
                        lightcolor="#4a4a4a",
                        darkcolor="#4a4a4a")
        
        style.configure("Horizontal.TScrollbar",
                        background="#4a4a4a",
                        troughcolor="#2b2b2b",
                        bordercolor="#2b2b2b",
                        arrowcolor="#d4d4d4",
                        lightcolor="#4a4a4a",
                        darkcolor="#4a4a4a")

        # Configure Notebook Tabs to match dark theme
        style.configure("TNotebook", background="#2b2b2b", borderwidth=0)
        style.configure("TNotebook.Tab", background="#3c3c3c", foreground="#d4d4d4", padding=[10, 5])
        style.map("TNotebook.Tab", background=[("selected", "#1e1e1e")], foreground=[("selected", "#ffffff")])
        
        # Build UI
        self.build_ui()
        
    def build_ui(self):
        """Build main UI layout"""
        # Title frame
        title_frame = tk.Frame(self.root, bg="#1e1e1e", height=50)
        title_frame.pack(fill=tk.X)
        title_frame.pack_propagate(False)
        
        title_label = tk.Label(
            title_frame, 
            text="JHAMELA Compiler - ঝামেলা কম্পাইলার by Salman & Akash",
            font=("Arial", 16, "bold"),
            fg="#ffffff",
            bg="#1e1e1e"
        )
        title_label.pack(side=tk.LEFT, padx=20, pady=10)
        
        # Main content frame
        content_frame = tk.Frame(self.root, bg="#2b2b2b")
        content_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # ===== LEFT PANE: INPUT =====
        left_frame = tk.LabelFrame(
            content_frame, 
            text="📝 Input Code (.jh)", 
            bg="#2b2b2b", 
            fg="#d4d4d4", 
            bd=1, 
            relief=tk.SOLID,
            highlightbackground="#444444",
            font=("Arial", 10, "bold")
        )
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(0, 5))
        
        # Input editor with scrollbar
        self.input_text = scrolledtext.ScrolledText(
            left_frame,
            wrap=tk.WORD,
            font=("Courier New", 11),
            bg="#1e1e1e",  # Dark Gray (Not pure black)
            fg="#e0e0e0",  # Off-White Text
            insertbackground="#e0e0e0",
            undo=True,
            maxundo=-1,
            bd=0,
            highlightthickness=0
        )
        self.input_text.pack(fill=tk.BOTH, expand=True, padx=2, pady=2)
        
        # Load sample code
        sample_code = """ধরো পূর্ণসংখ্যা ক;
রাখো ক = ৫;
দেখাও ক;

যদি ক > ১০ {
    দেখাও "সংখ্যাটি বড়";
} নাহলে {
    দেখাও "সংখ্যাটি ছোট";
}

যতক্ষণ ক > ০ {
    দেখাও ক;
    রাখো ক = ক - ২;
}
"""
        self.input_text.insert(tk.END, sample_code)
        
        # ===== RIGHT PANE: OUTPUT =====
        right_frame = tk.LabelFrame(
            content_frame, 
            text="📊 Output", 
            bg="#2b2b2b", 
            fg="#d4d4d4", 
            bd=1, 
            relief=tk.SOLID,
            highlightbackground="#444444",
            font=("Arial", 10, "bold")
        )
        right_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=(5, 0))
        
        # Output tabs
        self.notebook = ttk.Notebook(right_frame)
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=2, pady=2)
        
        # Tab 1: Tokens
        tokens_frame = tk.Frame(self.notebook, bg="#1e1e1e")
        self.notebook.add(tokens_frame, text="🔤 Tokens")
        self.tokens_text = scrolledtext.ScrolledText(
            tokens_frame,
            wrap=tk.WORD,
            font=("Courier New", 10),
            bg="#1e1e1e",
            fg="#e0e0e0",
            state=tk.DISABLED,
            bd=0,
            highlightthickness=0
        )
        self.tokens_text.pack(fill=tk.BOTH, expand=True)
        
        # Tab 2: AST
        ast_frame = tk.Frame(self.notebook, bg="#1e1e1e")
        self.notebook.add(ast_frame, text="🌳 Parse Tree")
        self.ast_text = scrolledtext.ScrolledText(
            ast_frame,
            wrap=tk.WORD,
            font=("Courier New", 10),
            bg="#1e1e1e",
            fg="#e0e0e0",
            state=tk.DISABLED,
            bd=0,
            highlightthickness=0
        )
        self.ast_text.pack(fill=tk.BOTH, expand=True)
        
        # Tab 3: Python Code
        python_frame = tk.Frame(self.notebook, bg="#1e1e1e")
        self.notebook.add(python_frame, text="🐍 Python Code")
        self.python_text = scrolledtext.ScrolledText(
            python_frame,
            wrap=tk.WORD,
            font=("Courier New", 10),
            bg="#1e1e1e",
            fg="#e0e0e0",
            state=tk.DISABLED,
            bd=0,
            highlightthickness=0
        )
        self.python_text.pack(fill=tk.BOTH, expand=True)
        
        # Tab 4: Execution Output
        exec_frame = tk.Frame(self.notebook, bg="#1e1e1e")
        self.notebook.add(exec_frame, text="▶️ Execution Output")
        self.exec_text = scrolledtext.ScrolledText(
            exec_frame,
            wrap=tk.WORD,
            font=("Courier New", 10),
            bg="#1e1e1e",
            fg="#e0e0e0",
            state=tk.DISABLED,
            bd=0,
            highlightthickness=0
        )
        self.exec_text.pack(fill=tk.BOTH, expand=True)
        
        # ===== BUTTON FRAME =====
        button_frame = tk.Frame(self.root, bg="#2b2b2b")
        button_frame.pack(fill=tk.X, padx=10, pady=10)
        
        # Compile button
        compile_btn = tk.Button(
            button_frame,
            text="⚙️  Compile",
            command=self.compile_code,
            font=("Arial", 11, "bold"),
            bg="#1e3a8a", # Dark Blue
            fg="white",
            activebackground="#1e40af",
            activeforeground="white",
            padx=20,
            pady=10,
            relief=tk.RAISED,
            cursor="hand2"
        )
        compile_btn.pack(side=tk.LEFT, padx=5)
        
        # Run button
        run_btn = tk.Button(
            button_frame,
            text="▶️  Run",
            command=self.run_python,
            font=("Arial", 11, "bold"),
            bg="#1d4ed8", # Dark Blue
            fg="white",
            activebackground="#1e3a8a",
            activeforeground="white",
            padx=20,
            pady=10,
            relief=tk.RAISED,
            cursor="hand2"
        )
        run_btn.pack(side=tk.LEFT, padx=5)
        
        # Save button
        save_btn = tk.Button(
            button_frame,
            text="💾 Save Input",
            command=self.save_file,
            font=("Arial", 11, "bold"),
            bg="#2563eb", # Dark Blue
            fg="white",
            activebackground="#1d4ed8",
            activeforeground="white",
            padx=20,
            pady=10,
            relief=tk.RAISED,
            cursor="hand2"
        )
        save_btn.pack(side=tk.LEFT, padx=5)
        
        # Load button
        load_btn = tk.Button(
            button_frame,
            text="📂 Load File",
            command=self.load_file,
            font=("Arial", 11, "bold"),
            bg="#3b82f6", # Dark Blue
            fg="white",
            activebackground="#2563eb",
            activeforeground="white",
            padx=20,
            pady=10,
            relief=tk.RAISED,
            cursor="hand2"
        )
        load_btn.pack(side=tk.LEFT, padx=5)
        
        # Clear button
        clear_btn = tk.Button(
            button_frame,
            text="🗑️  Clear All",
            command=self.clear_all,
            font=("Arial", 11, "bold"),
            bg="#1e40af", # Dark Blue
            fg="white",
            activebackground="#0f172a",
            activeforeground="white",
            padx=20,
            pady=10,
            relief=tk.RAISED,
            cursor="hand2"
        )
        clear_btn.pack(side=tk.LEFT, padx=5)
        
        # ===== BOTTOM FRAME (Status + Credit) =====
        bottom_frame = tk.Frame(self.root, bg="#2b2b2b")
        bottom_frame.pack(fill=tk.X, side=tk.BOTTOM)
        
        # Status bar
        self.status_var = tk.StringVar(value="Ready")
        status_bar = tk.Label(
            bottom_frame,
            textvariable=self.status_var,
            bg="#2b2b2b",
            fg="#d4d4d4",
            pady=5,
            anchor=tk.W,
            padx=10
        )
        status_bar.pack(side=tk.LEFT, fill=tk.X, expand=True)
        
        # Credit Label (Rightmost corner below)
        credit_label = tk.Label(
            bottom_frame,
            text="by Salman Khan Rume & Akash Das © 2026",
            bg="#2b2b2b",
            fg="#888888",
            pady=5,
            padx=10
        )
        credit_label.pack(side=tk.RIGHT)
    
    def update_status(self, msg, color="#34495e"):
        """Update status bar"""
        self.status_var.set(msg)
        self.root.update()
    
    def set_text(self, widget, text):
        """Helper to set text in disabled widget"""
        widget.config(state=tk.NORMAL)
        widget.delete(1.0, tk.END)
        widget.insert(tk.END, text)
        widget.config(state=tk.DISABLED)
    
    def compile_code(self):
        """Compile JHAMELA code"""
        self.update_status("⏳ Compiling...")
        
        # Get input code
        code = self.input_text.get(1.0, tk.END).strip()
        if not code:
            messagebox.showwarning("Warning", "Input code is empty")
            self.update_status("Error: Empty input")
            return
        
        # Write to temp file
        try:
            with open(self.temp_input_file, "w", encoding="utf-8") as f:
                f.write(code)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to write temp file: {e}")
            self.update_status("Error: Write failed")
            return
        
        # Call compiler
        try:
            env = os.environ.copy()
            env["PYTHONIOENCODING"] = "utf-8"
            
            result = subprocess.run(
                [self.compiler_path, self.temp_input_file],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,    
                text=True,
                encoding='utf-8',
                env=env,
                timeout=5
            )
            
            output = result.stdout + result.stderr
            self.parse_compiler_output(output)
            
            self.update_status("✓ Compilation successful!")
            
        except FileNotFoundError:
            messagebox.showerror(
                "Error", 
                f"Compiler not found: {self.compiler_path}\n\n"
                "Ensure jhamela.exe is in the same directory or provide correct path."
            )
            self.update_status("Error: Compiler not found")
        except subprocess.TimeoutExpired:
            messagebox.showerror("Error", "Compilation timeout")
            self.update_status("Error: Timeout")
        except Exception as e:
            messagebox.showerror("Error", f"Compilation failed: {e}")
            self.update_status(f"Error: {str(e)}")
    
    def parse_compiler_output(self, output):
        """Parse and display compiler output in tabs"""
        lines = output.split("\n")
        
        tokens_section = []
        ast_section = []
        python_section = []
        
        current_section = None
        
        for line in lines:
            if "=== TOKENS ===" in line:
                current_section = "tokens"
            elif "=== PARSE TREE ===" in line:
                current_section = "ast"
            elif "=== CODE GENERATION ===" in line:
                current_section = "python"
            elif "=== COMPILATION SUCCESSFUL ===" in line:
                break
            elif current_section == "tokens" and line.strip():
                tokens_section.append(line)
            elif current_section == "ast" and line.strip():
                ast_section.append(line)
            elif current_section == "python" and line.strip():
                python_section.append(line)
        
        self.set_text(self.tokens_text, "\n".join(tokens_section))
        self.set_text(self.ast_text, "\n".join(ast_section))
        
        try:
            with open("output.py", "r", encoding="utf-8") as f:
                python_code = f.read()
            self.set_text(self.python_text, python_code)
        except Exception:
            self.set_text(self.python_text, "(Python file not generated)")
        
        self.set_text(self.exec_text, "(Run to see output)")
    
    def run_python(self):
        """Execute generated Python code"""
        self.update_status("⏳ Running...")
        
        try:
            if not os.path.exists("output.py"):
                messagebox.showwarning("Warning", "No compiled Python code. Compile first!")
                self.update_status("Error: No output.py")
                return
            
            env = os.environ.copy()
            env["PYTHONIOENCODING"] = "utf-8"
            
            result = subprocess.run(
                [sys.executable, "output.py"],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                encoding='utf-8',
                env=env,
                timeout=10
            )
            
            output = result.stdout
            if result.stderr:
                output += f"\n--- STDERR ---\n{result.stderr}"
            
            self.set_text(self.exec_text, output if output.strip() else "(No output)")
            self.update_status("✓ Execution complete!")
            
        except subprocess.TimeoutExpired:
            messagebox.showerror("Error", "Execution timeout")
            self.update_status("Error: Timeout")
        except Exception as e:
            messagebox.showerror("Error", f"Execution failed: {e}")
            self.update_status(f"Error: {str(e)}")
    
    def save_file(self):
        """Save input code to file"""
        filepath = filedialog.asksaveasfilename(
            defaultextension=".jh",
            filetypes=[("JHAMELA files", "*.jh"), ("All files", "*.*")]
        )
        
        if filepath:
            try:
                with open(filepath, "w", encoding="utf-8") as f:
                    f.write(self.input_text.get(1.0, tk.END))
                messagebox.showinfo("Success", f"Saved to {filepath}")
                self.update_status(f"Saved: {filepath}")
            except Exception as e:
                messagebox.showerror("Error", f"Save failed: {e}")
    
    def load_file(self):
        """Load input code from file"""
        filepath = filedialog.askopenfilename(
            filetypes=[("JHAMELA files", "*.jh"), ("All files", "*.*")]
        )
        
        if filepath:
            try:
                with open(filepath, "r", encoding="utf-8") as f:
                    code = f.read()
                self.input_text.delete(1.0, tk.END)
                self.input_text.insert(tk.END, code)
                messagebox.showinfo("Success", f"Loaded from {filepath}")
                self.update_status(f"Loaded: {filepath}")
            except Exception as e:
                messagebox.showerror("Error", f"Load failed: {e}")
    
    def clear_all(self):
        """Clear all text areas"""
        if messagebox.askyesno("Confirm", "Clear all content?"):
            self.input_text.delete(1.0, tk.END)
            self.set_text(self.tokens_text, "")
            self.set_text(self.ast_text, "")
            self.set_text(self.python_text, "")
            self.set_text(self.exec_text, "")
            self.update_status("Cleared")


def main():
    root = tk.Tk()
    
    compiler_path = None
    if len(sys.argv) > 1:
        compiler_path = sys.argv[1]
    elif os.path.exists("jhamela.exe"):
        compiler_path = "jhamela.exe"
    
    app = JhamelaCompilerGUI(root, compiler_path)
    root.mainloop()


if __name__ == "__main__":
    main()
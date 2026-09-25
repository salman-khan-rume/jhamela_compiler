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

        self.compiler_path = compiler_exe_path or "jhamela.exe"
        self.temp_input_file = "temp_input.jh"

        self.root.configure(bg="#2b2b2b")
        style = ttk.Style()
        style.theme_use('clam')

        style.configure("Vertical.TScrollbar",
                        background="#4a4a4a", troughcolor="#2b2b2b",
                        bordercolor="#2b2b2b", arrowcolor="#d4d4d4",
                        lightcolor="#4a4a4a", darkcolor="#4a4a4a")
        style.configure("Horizontal.TScrollbar",
                        background="#4a4a4a", troughcolor="#2b2b2b",
                        bordercolor="#2b2b2b", arrowcolor="#d4d4d4",
                        lightcolor="#4a4a4a", darkcolor="#4a4a4a")
        style.configure("TNotebook", background="#2b2b2b", borderwidth=0)
        style.configure("TNotebook.Tab", background="#3c3c3c",
                        foreground="#d4d4d4", padding=[10, 5])
        style.map("TNotebook.Tab",
                  background=[("selected", "#1e1e1e")],
                  foreground=[("selected", "#ffffff")])

        self.build_ui()

    def build_ui(self):
        title_frame = tk.Frame(self.root, bg="#1e1e1e", height=50)
        title_frame.pack(fill=tk.X)
        title_frame.pack_propagate(False)

        tk.Label(title_frame,
                 text="JHAMELA Compiler - ঝামেলা কম্পাইলার by Salman & Akash",
                 font=("Arial", 16, "bold"),
                 fg="#ffffff", bg="#1e1e1e").pack(side=tk.LEFT, padx=20, pady=10)

        content_frame = tk.Frame(self.root, bg="#2b2b2b")
        content_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # LEFT: INPUT
        left_frame = tk.LabelFrame(content_frame, text="📝 Input Code (.jh)",
                                   bg="#2b2b2b", fg="#d4d4d4", bd=1,
                                   relief=tk.SOLID, highlightbackground="#444444",
                                   font=("Arial", 10, "bold"))
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(0, 5))

        self.input_text = scrolledtext.ScrolledText(
            left_frame, wrap=tk.WORD, font=("Courier New", 11),
            bg="#1e1e1e", fg="#e0e0e0", insertbackground="#e0e0e0",
            undo=True, maxundo=-1, bd=0, highlightthickness=0)
        self.input_text.pack(fill=tk.BOTH, expand=True, padx=2, pady=2)

        sample_code = """ধরো পূর্ণসংখ্যা ক;
ধরো সত্যমূল্য বড়;

রাখো ক = ৫;
রাখো বড় = ক > ৩;

দেখাও ক;
দেখাও বড়;

যদি বড় {
    দেখাও "সংখ্যাটি বড়";
} নাহলে {
    দেখাও "সংখ্যাটি ছোট";
}

যতক্ষণ ক > ০ {
    দেখাও ক;
    রাখো ক = ক - ১;
}
"""
        self.input_text.insert(tk.END, sample_code)

        # RIGHT: OUTPUT
        right_frame = tk.LabelFrame(content_frame, text="📊 Output",
                                    bg="#2b2b2b", fg="#d4d4d4", bd=1,
                                    relief=tk.SOLID, highlightbackground="#444444",
                                    font=("Arial", 10, "bold"))
        right_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=(5, 0))

        self.notebook = ttk.Notebook(right_frame)
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=2, pady=2)

        def make_tab(label):
            frame = tk.Frame(self.notebook, bg="#1e1e1e")
            self.notebook.add(frame, text=label)
            widget = scrolledtext.ScrolledText(
                frame, wrap=tk.WORD, font=("Courier New", 10),
                bg="#1e1e1e", fg="#e0e0e0", state=tk.DISABLED,
                bd=0, highlightthickness=0)
            widget.pack(fill=tk.BOTH, expand=True)
            return widget

        self.tokens_text = make_tab("🔤 Tokens")
        self.ast_text    = make_tab("🌳 Parse Tree")
        self.python_text = make_tab("🐍 Python Code")
        self.exec_text   = make_tab("▶️ Execution Output")

        # BUTTONS
        button_frame = tk.Frame(self.root, bg="#2b2b2b")
        button_frame.pack(fill=tk.X, padx=10, pady=10)

        def make_btn(text, command, bg):
            return tk.Button(button_frame, text=text, command=command,
                             font=("Arial", 11, "bold"), bg=bg, fg="white",
                             activebackground=bg, activeforeground="white",
                             padx=20, pady=10, relief=tk.RAISED, cursor="hand2")

        make_btn("⚙️  Compile",     self.compile_code, "#1e3a8a").pack(side=tk.LEFT, padx=5)
        make_btn("▶️  Run",         self.run_python,   "#1d4ed8").pack(side=tk.LEFT, padx=5)
        make_btn("💾 Save Input",   self.save_file,    "#2563eb").pack(side=tk.LEFT, padx=5)
        make_btn("📂 Load File",    self.load_file,    "#3b82f6").pack(side=tk.LEFT, padx=5)
        make_btn("🗑️  Clear All",   self.clear_all,    "#1e40af").pack(side=tk.LEFT, padx=5)

        # BOTTOM
        bottom_frame = tk.Frame(self.root, bg="#2b2b2b")
        bottom_frame.pack(fill=tk.X, side=tk.BOTTOM)

        self.status_var = tk.StringVar(value="Ready")
        tk.Label(bottom_frame, textvariable=self.status_var, bg="#2b2b2b",
                 fg="#d4d4d4", pady=5, anchor=tk.W, padx=10).pack(
                     side=tk.LEFT, fill=tk.X, expand=True)
        tk.Label(bottom_frame, text="by Salman Khan Rume & Akash Das © 2026",
                 bg="#2b2b2b", fg="#888888", pady=5, padx=10).pack(side=tk.RIGHT)

    def update_status(self, msg):
        self.status_var.set(msg)
        self.root.update()

    def set_text(self, widget, text):
        widget.config(state=tk.NORMAL)
        widget.delete(1.0, tk.END)
        widget.insert(tk.END, text)
        widget.config(state=tk.DISABLED)

    def compile_code(self):
        self.update_status("⏳ Compiling...")
        code = self.input_text.get(1.0, tk.END).strip()
        if not code:
            messagebox.showwarning("Warning", "Input code is empty")
            self.update_status("Error: Empty input")
            return

        try:
            with open(self.temp_input_file, "w", encoding="utf-8") as f:
                f.write(code)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to write temp file: {e}")
            self.update_status("Error: Write failed")
            return

        try:
            env = os.environ.copy()
            env["PYTHONIOENCODING"] = "utf-8"

            result = subprocess.run(
                [self.compiler_path, self.temp_input_file],
                stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                text=True, encoding='utf-8', env=env, timeout=5)

            output = result.stdout + result.stderr
            self.parse_compiler_output(output)
            self.update_status("✓ Compilation successful!")

        except FileNotFoundError:
            messagebox.showerror(
                "Error",
                f"Compiler not found: {self.compiler_path}\n\n"
                "Ensure jhamela.exe is in the same directory or provide correct path.")
            self.update_status("Error: Compiler not found")
        except subprocess.TimeoutExpired:
            messagebox.showerror("Error", "Compilation timeout")
            self.update_status("Error: Timeout")
        except Exception as e:
            messagebox.showerror("Error", f"Compilation failed: {e}")
            self.update_status(f"Error: {str(e)}")

    def parse_compiler_output(self, output):
        lines = output.split("\n")
        tokens_section = []
        ast_section = []
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
                stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                text=True, encoding='utf-8', env=env, timeout=10)

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
        filepath = filedialog.asksaveasfilename(
            defaultextension=".jh",
            filetypes=[("JHAMELA files", "*.jh"), ("All files", "*.*")])
        if filepath:
            try:
                with open(filepath, "w", encoding="utf-8") as f:
                    f.write(self.input_text.get(1.0, tk.END))
                messagebox.showinfo("Success", f"Saved to {filepath}")
                self.update_status(f"Saved: {filepath}")
            except Exception as e:
                messagebox.showerror("Error", f"Save failed: {e}")

    def load_file(self):
        filepath = filedialog.askopenfilename(
            filetypes=[("JHAMELA files", "*.jh"), ("All files", "*.*")])
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
    JhamelaCompilerGUI(root, compiler_path)
    root.mainloop()


if __name__ == "__main__":
    main()
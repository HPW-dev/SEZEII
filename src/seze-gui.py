#!/usr/bin/env python
from os import system
from platform import architecture
import tkinter as tk
import os.path as path
import sys
from tkinter import filedialog
#from typing_extensions import ParamSpecArgs

is_linux = False
input_video = ""
output_video = ""
plugin_opts = ""
plugin_name = ""
window = None
entry_in_vid = None
button_in_vid = None
entry_out_vid = None
button_out_vid = None
entry_plug = None
entry_opts = None
without_output = None
without_threads = None

# диалог открытия файла
def getLoadFileName(wnd):
  name = filedialog.askopenfilename(
    parent = wnd,
    initialdir = ".",
    title = "Load as",
  )
  return name

def getSaveFileName(wnd):
  name = filedialog.asksaveasfilename(
    parent = wnd,
    initialdir = ".",
    title = "Save as"
  )
  return name

def load_in():
  txt = getLoadFileName(window)
  entry_in_vid.insert(0, txt)

def load_out():
  txt = getSaveFileName(window)
  entry_out_vid.insert(0, txt)

def load_plug():
  plugin_name = getLoadFileName(window)
  entry_plug.insert(0, plugin_name)

def process():
  input_video = entry_in_vid.get()
  output_video = entry_out_vid.get()
  plugin_name = entry_plug.get()
  plugin_opts = entry_opts.get()
  output = f" -o \"{output_video}\"" if (without_output.get() == 0) \
    else " --nout"
  separator = "/" if is_linux else "\\"
  command = f".{separator}seze -i \"{input_video}\"" \
    f"{output} --plug \"{plugin_name}\" --opts " \
    f"\"{plugin_opts}\""
  if (without_threads.get() != 0):
    command += " -j 1"
  print(f"calling {command}\n")
  system(command)

def make_loader(master, title, cmd=None):
  frame = tk.Frame(master, borderwidth=5)
  frame.pack(expand=False, side=tk.TOP, fill=tk.BOTH)
  button = tk.Button(master=frame, text=title, command=cmd)
  button.pack(expand=False, side=tk.LEFT)
  entry = tk.Entry(master=frame)
  entry.pack(expand=True, side=tk.RIGHT, fill=tk.BOTH)
  return button, entry

def make_entry(master, title):
  frame = tk.Frame(master, borderwidth=5)
  frame.pack(expand=False, side=tk.TOP, fill=tk.BOTH)
  label = tk.Label(master=frame, text=title)
  label.pack(expand=False, side=tk.LEFT)
  entry = tk.Entry(master=frame)
  entry.pack(expand=True, side=tk.RIGHT, fill=tk.BOTH)
  return entry

def make_button(master, title, cmd):
  frame = tk.Frame(master, borderwidth=5)
  frame.pack(expand=False, side=tk.BOTTOM, fill=tk.BOTH)
  button = tk.Button(master=frame, text=title, command=cmd, width=10)
  button.pack(expand=False, side=tk.RIGHT)
  return button

def disable_inactive():
  entry_out_vid["state"] = tk.NORMAL if (without_output.get() == 0) else tk.DISABLED;
  button_out_vid["state"] = tk.NORMAL if (without_output.get() == 0) else tk.DISABLED;

def make_checkbox(master, title):
  frame = tk.Frame(master)
  frame.pack(expand=False, side=tk.TOP, fill=tk.BOTH)
  ret = tk.IntVar()
  checkbox = tk.Checkbutton(master=frame, text=title, variable=ret,
    onvalue=1, offvalue=0, command=disable_inactive)
  checkbox.pack(expand=False, side=tk.LEFT)
  return ret

if __name__ == '__main__':

  arch_raw = architecture()
  is_linux = True if arch_raw[1] == "ELF" else False

  window = tk.Tk()
  window.title("SEZE II")
  window.geometry("400x220")

  button_in_vid, entry_in_vid = make_loader(window, "input video", load_in)
  without_output = make_checkbox(window, "without output video")
  button_out_vid, entry_out_vid = make_loader(window, "output video", load_out)
  _, entry_plug = make_loader(window, "plugin file", load_plug)
  entry_opts = make_entry(window, "plugin options ")
  without_threads = make_checkbox(window, "without multithread processing")
  make_button(window, "START", process)

  window.mainloop()

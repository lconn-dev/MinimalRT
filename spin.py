import subprocess, os, shutil, time
from subprocess import Popen
from shutil import copyfile

print("Spin")

dir_path = os.path.dirname(os.path.realpath(__file__))
exePath = "/out/build/x64-Release/"
exeName = "raytracer.exe"

angle = 1
maxAngle = 300

#print ("Path: ", exePath)


while angle < maxAngle:
  print("ANGLE:", angle)
  proc = subprocess.Popen([dir_path+exePath+exeName, "-rotate", str(angle)], cwd=dir_path+exePath);
  proc.wait();
  copyfile(dir_path + exePath + "output.tga", dir_path + exePath + "frames/" + str(angle) + ".tga")
  # cool down for a few seconds
  time.sleep(0.5)
  angle += 15

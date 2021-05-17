#!/bin/bash
#shell script to open two terminals to demonstrate the use of the text-editor
for i in `seq 1 2`;
  do
  gnome-terminal -- bash -c "make run; exec bash"
done

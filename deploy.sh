#! /bin/sh

SSHCMD='ssh -T root@10.0.0.30'

echo "Backing up existing sketch on target..."
echo 'mv -f /sketch/sketch.elf /sketch/sketch.elf.old' | $SSHCMD
echo "Stopping sketch process..."
echo 'killall sketch.elf' | $SSHCMD
echo "Sending new sketch..."
scp Release/Robot-2015.elf root@10.0.0.30:/sketch/sketch.elf
echo "Done, press the reset button on the board"

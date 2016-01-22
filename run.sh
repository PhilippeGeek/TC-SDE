#!/bin/bash
x-terminal-emulator --geometry=512x512+120+0 --title="Gestion des Feux" -e `dirname $0`/feux > /dev/null 2> /dev/null &
x-terminal-emulator --geometry=512x512+632+0 --title="Générateur de trafic" -e `dirname $0`/generateur_trafic > /dev/null 2> /dev/null  &
x-terminal-emulator --geometry=512x512+120+512 --title="Générateur de trafic (Prio)" -e `dirname $0`/generateur_trafic_prioritaire > /dev/null 2> /dev/null  &
x-terminal-emulator --geometry=512x512+632+512 --title="Coordinateur" -e `dirname $0`/coordinateur > /dev/null 2> /dev/null  &
echo "Programme lancé!";
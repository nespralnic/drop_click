#!/bin/bash

# Check if sdl2-config command is available
if ! command -v sdl2-config &> /dev/null; then
    echo "⚠️ SDL2 is not installed. Installing now..."
    sudo apt update && sudo apt install libsdl2-dev libsdl2-2.0-0 -y
else
    echo "✅ SDL2 is already installed. Version: $(sdl2-config --version)"
fi

# TicketMachine
## Prerequisite (especially for linux)
- Have a conan installed on your PC.
- Have a default conan profile. If not present, use:
`conan profile detect`
- Have libs installed:
    libgl1-mesa-dev, 
    libx11-dev, 
    libxrandr-dev, 
    libudev-dev,
## Running
`cmake -B build -DCMAKE_BUILD_TYPE=Release` or `cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release` (if ninja present)
`cmake --build build`

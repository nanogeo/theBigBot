#!/bin/bash

# Run WSL with Ubuntu-20.04 and execute the commands
wsl -d Ubuntu-20.04 << 'EOF'

# Configure the build with cmake
cmake -B build -DBUILD_FOR_LADDER=ON -DSC2_VERSION=5.0.12

# Build the project with 8 parallel jobs
cmake --build build -j8 2>&1 | tee build.log

# Check if the build failed
if [ $? -ne 0 ]; then
    echo "Build failed. Check the log above or in 'build.log' for details."
else
    echo "Build completed successfully."
fi
EOF

# Keep the window open
echo "Press any key to close..."
read -n 1
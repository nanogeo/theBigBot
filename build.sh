#!/bin/bash

# Run WSL with Ubuntu-20.04 and execute the commands
wsl -d Ubuntu-20.04 << 'EOF'

# Remove the build/bin folder if it exists
if [ -d build/bin ]; then
  echo "Removing existing build/bin directory..."
  rm -rf build/bin
fi

# Configure the build with cmake
cmake -B build -DBUILD_FOR_LADDER=ON -DSC2_VERSION=5.0.12

# Build the project
cmake --build build -j8 2>&1 | tee build.log

# Check if the output file exists
if [ -f build/bin/theBigBot ]; then
  # Zip the output file
  zip -j build/bin/theBigBot.zip build/bin/theBigBot
  echo "Zipped theBigBot to build/bin/theBigBot.zip"
else
  echo "Error: theBigBot file not found in build/bin/"
  exit 1
fi

echo "Build and packaging completed successfully."
EOF

# Keep the window open
echo "Press any key to close..."
read -n 1
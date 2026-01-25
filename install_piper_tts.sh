#!/usr/bin/env bash
#
# install_piper_tts.sh - Fresh install of Piper TTS 1.2.0 ARM64
# Installs binary + libs directly in /opt/piper/bin/
# Voice model in /opt/piper/voices/
# No other subdirectories
# Author: N5AD - January 2026
set -euo pipefail

echo "Starting fresh Piper TTS 1.2.0 ARM64 install..."



# 1. Download Piper 1.2.0 ARM64 binary tarball
echo "Downloading Piper binary..."
sudo wget https://github.com/rhasspy/piper/releases/download/v1.2.0/piper_arm64.tar.gz -O /tmp/piper.tar.gz

# 2. Create /opt/piper/bin
sudo mkdir -p /opt/piper/bin

# 3. Extract EVERYTHING directly into /opt/piper/bin (binary + all .so libs)
echo "Extracting Piper files directly into /opt/piper/bin..."
sudo tar -xzf /tmp/piper.tar.gz -C /opt/piper/bin

# 4. Make the binary executable
sudo chmod +x /opt/piper/bin/piper

# 5. Create voices directory
sudo mkdir -p /opt/piper/voices

# 6. Download Lessac Medium voice model
cd /opt/piper/voices
echo "Downloading Lessac Medium voice model..."
sudo wget -4 https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx
sudo wget -4 https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json

# 7. Set correct ownership & permissions
sudo chown -R www-data:www-data /opt/piper
sudo chmod -R 755 /opt/piper/bin
sudo chmod -R 644 /opt/piper/voices/*

# 9. Test the installation
echo "Testing Piper installation..."
/opt/piper/bin/piper/piper --version

# Generate a test WAV
echo "This is a test of Piper TTS on node $(hostname)" | /opt/piper/bin/piper/piper --model /opt/piper/voices/en_US-lessac-medium.onnx --output_file /mp3/piper_test.wav

# Check the test file
ls -l /mp3/piper_test.wav

echo ""
echo "Piper TTS 1.2.0 ARM64 install complete!"
echo "Structure:"
ls -l /opt/piper/
echo ""
echo "Test file created at /mp3/piper_test.wav"
echo ""
echo "73 — N5AD"

#!/bin/bash

# 设置NDK路径
NDK_PATH="/Users/lantianbao/Library/Android/sdk/ndk/16.1.4479499"
CMAKE_PATH="$NDK_PATH/build/cmake/android.toolchain.cmake"

# 支持的架构
ABIS=("armeabi-v7a" "arm64-v8a" "x86" "x86_64")

# 输出目录
OUTPUT_DIR="mglogger/src/main/cpp/jniLibs"

# 为每个架构编译
for ABI in "${ABIS[@]}"; do
    echo "Building for $ABI..."
    
    # 创建构建目录
    BUILD_DIR="build_external_$ABI"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # 配置CMake
    cmake -DCMAKE_TOOLCHAIN_FILE="$CMAKE_PATH" \
          -DANDROID_ABI="$ABI" \
          -DANDROID_PLATFORM=android-21 \
          -DCMAKE_BUILD_TYPE=Release \
          ../mglogger/src/main/cpp/external
    
    # 编译
    make -j$(nproc)
    
    # 复制生成的库文件
    cp "libmgmbedtls.so" "../$OUTPUT_DIR/$ABI/"
    
    # 清理构建目录
    cd ..
    rm -rf "$BUILD_DIR"
    
    echo "Built for $ABI completed"
done

echo "All external libraries built successfully!" 
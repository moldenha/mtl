Examples of using metal-cpp


How to make sure it works out of the box:
    - Ensure developer tools are installed: `sudo xcode-select --install`
    - Make sure to install the `XCode` app
    - Select XCode as the active developer directory: `sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer`
    - Verify with: `xcrun --find metal`
    - Ensure you accept XCode license: `xcrun --find metal`
    - You may also need to download the metal toolchain with this: `xcodebuild -downloadComponent MetalToolchain`
    - From there, cd to the the examples and run:
        ```
        cmake -S . -B build
        cd build
        cmake --build . --config Release
        ./[target]
        ```

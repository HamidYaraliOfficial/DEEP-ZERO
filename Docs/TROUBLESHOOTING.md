# Troubleshooting

## UBT errors
Delete `Binaries`, `Intermediate` and `Saved`, regenerate project files, rebuild.

## SDK errors
Verify Visual Studio Game development with C++, Desktop development with C++ and a Windows SDK.

## Missing plugins
Ensure Enhanced Input and Niagara are enabled in the project.

## Shaders
First launch may compile Lumen/Nanite shaders and populate the Derived Data Cache.

## Packaging
Inspect UAT and `Saved/Logs` from the first error, not the final cascade message.

B:\VulkanSDK\1.1.70.1\Bin32\spirv-opt.exe --strip-debug --inline-entry-points-exhaustive --convert-local-access-chains --eliminate-local-single-block --eliminate-local-single-store --eliminate-insert-extract --eliminate-dead-code-aggressive --eliminate-dead-branches --merge-blocks --eliminate-local-single-block --eliminate-local-single-store --eliminate-local-multi-store --eliminate-insert-extract --eliminate-dead-code-aggressive --eliminate-common-uniform vertdbg.spv -o vertdbg-sm.spv
REM B:\VulkanSDK\1.1.70.1\Bin32\glslangValidator.exe -Os -o fragdbg.spv -V shaderdbg.frag
REM B:\VulkanSDK\1.1.70.1\Bin32\glslangValidator.exe -Os -V shader.vert
REM B:\VulkanSDK\1.1.70.1\Bin32\glslangValidator.exe -Os -V shader.frag
pause
param (
    [switch]$run = $false,
    [switch]$build = $false,
    [switch]$clean = $false,
    [switch]$shaders = $false
)
# Assumes this script is in root directory

function build_shaders ()
{
    # go to shaders directory
    Set-Location -Path "src/shaders"

    glslc blinn.vert -o blinn-vertex.spv
    glslc blinn.frag -o blinn-fragment.spv

    glslc env.vert -o env-vertex.spv
    glslc env.frag -o env-fragment.spv

    Set-Location -Path "../.."
}

function build
{
    Write-Output "building!"

    Set-Location -Path "build"
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .. -G Ninja
    cmake --build . --target vulkan-engine
    Set-Location -Path ".."

    build_shaders
}

function clean
{
    Write-Output "cleaning!"

    Remove-Item -Recurse -Force build
    New-Item -Path "build" -ItemType Directory

    Remove-Item src/shaders/*.spv
}

function run
{
    Write-Output "running!"
    ./build/vulkan-engine 
}

if($run)
{
    run
} elseif ($build)
{
    build
} elseif($clean)
{
    clean
} elseif($shaders)
{
    build_shaders
} else
{
    build
    run
}

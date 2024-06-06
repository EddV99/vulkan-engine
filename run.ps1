param (
    [switch]$run = $false,
    [switch]$build = $false,
    [switch]$clean = $false,
    [switch]$shaders = $false
)
# Assumes this script is in root directory

# do everything in the build directory
#Set-Location -Path "build"

function build_shaders ()
{
    # go to shaders directory
    Set-Location -Path "src/shaders"

    glslc basic.vert -o vert.spv
    glslc basic.frag -o frag.spv

    # go back to build directory
    Set-Location -Path "../.."
}

function build
{
    Write-Output "building!"

    Set-Location -Path "build"
    cmake .. 
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
    ./vulkan-engine
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

#Set-Location -Path ".."

#if [[ $1 == "run" ]]; then
#run
#elif [[ $1 == "build" ]]; then
#build
#elif [[ $1 == "clean" ]]; then
#clean
#else
#build
#run
#fi

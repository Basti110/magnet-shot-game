if [ $# -ne 1 ]
    then
        echo "No argument provided!"
        echo "Usage: export.sh mylevel.blend"
        exit
fi

# start blender in background, load scene, and run exporter
blender -b $1 -P exporter.py

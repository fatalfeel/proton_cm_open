#!/bin/bash

echo Process our images and textures and copy them into the bin directory

process_directory_images()
{
    for IMG in `find . -depth \( -name '*.x' -o -name '*.b3d' -o -name '*.bsp' -o -name '*.obj' \) -print`;
    do
    {
        ZIPFILE=`echo $IMG | sed -e 's/\.[^.]*$//'`
        ../../shared/OSX/utils/7za a "$ZIPFILE".zip "$IMG"
    }
    done
}

process_directory_images

echo Copy the stuff we care about

copy_media_to_bin()
{
    if [[ -d "$1" ]]; then
    sed -e 's/^\..*$/*&/g' "$2" | rsync -v --update --delete --delete-excluded --recursive --exclude-from=- "$1" ../bin
    fi
}

copy_media_to_bin interface exclude.txt
copy_media_to_bin audio     exclude.txt
copy_media_to_bin game      game_exclude.txt
copy_media_to_bin shaders   game_exclude.txt

remove_media_zip()
{
    for IMG in `find . -depth \( -name '*.zip' \) -print`;
    do
    {
        ZIPFILE=`echo $IMG`
        rm "$ZIPFILE"
    }
    done
}
remove_media_zip

remove_bin_zip()
{
    for IMG in `find ../bin/game/house_scene -depth \( -name '*.zip' \) -print`;
    do
    {
        ZIPFILE=`echo $IMG`
        rm "$ZIPFILE"
    }
    done

    for IMG in `find ../bin/game/quake -depth \( -name '*.zip' \) -print`;
    do
    {
        ZIPFILE=`echo $IMG`
        rm "$ZIPFILE"
    }
    done
}
remove_bin_zip

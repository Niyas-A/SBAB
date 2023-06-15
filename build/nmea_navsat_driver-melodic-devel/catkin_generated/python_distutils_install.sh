#!/bin/sh

if [ -n "$DESTDIR" ] ; then
    case $DESTDIR in
        /*) # ok
            ;;
        *)
            /bin/echo "DESTDIR argument must be absolute... "
            /bin/echo "otherwise python's distutils will bork things."
            exit 1
    esac
fi

echo_and_run() { echo "+ $@" ; "$@" ; }

echo_and_run cd "/home/sbclab/SBAB/src/nmea_navsat_driver-melodic-devel"

# ensure that Python install destination exists
echo_and_run mkdir -p "$DESTDIR/home/sbclab/SBAB/install/lib/python2.7/dist-packages"

# Note that PYTHONPATH is pulled from the environment to support installing
# into one location when some dependencies were installed in another
# location, #123.
echo_and_run /usr/bin/env \
    PYTHONPATH="/home/sbclab/SBAB/install/lib/python2.7/dist-packages:/home/sbclab/SBAB/build/lib/python2.7/dist-packages:$PYTHONPATH" \
    CATKIN_BINARY_DIR="/home/sbclab/SBAB/build" \
    "/usr/bin/python2" \
    "/home/sbclab/SBAB/src/nmea_navsat_driver-melodic-devel/setup.py" \
     \
    build --build-base "/home/sbclab/SBAB/build/nmea_navsat_driver-melodic-devel" \
    install \
    --root="${DESTDIR-/}" \
    --install-layout=deb --prefix="/home/sbclab/SBAB/install" --install-scripts="/home/sbclab/SBAB/install/bin"

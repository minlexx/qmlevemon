Extra openssl libs are required to support HTTPS - in arm-linux-androideabi-4.9 dir.
Add them as extra dependencies in qt creator project build section

You need also to manually patch generated json in build directory:
android-libqmlevemon_android.so-deployment-settings.json, change 
"qml-root-path": "V:/dev/QMLEVEMon/qmlevemon/android",
to
"qml-root-path": "V:/dev/QMLEVEMon/qmlevemon/qml",
TEMPLATE = subdirs
SUBDIRS = \
    actions \
    audio \
    audio/rtmidi \
    dialogs \
    painters \
    powertabdocument \
    widgets \
    app \
    build # this must be last since it depends on all previous subdirs

CONFIG += ordered
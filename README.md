# Yet Another Spotify Tray

This application should not exist. Spotify should have this feature automatically.
This application adds a tray icon to the Spotify Linux client application.

## Features

* Launches Spotify minimized to the system tray.

* Provides basic playback control through right-click menu.

## Installation

* Clone this repository.

* Build the application:

  ```
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  make
  ```

* Install the application:

  ```
  sudo make install
  ```

## Dependencies

* Qt5
* Xlib
* CMake

## Usage

Just start Spotify *Music Player in System Tray* from the application menu. Alternatively you may star if from a command line as
`spotify-tray`. Command-line arguments are passed directly to Spotify with the exception of`--hide-window` <u>given as the first
argument</u>, in which case the Spotify window is hidden at startup.

To have the window hidden by default, edit the `spotify-tray.desktop` file and change the line

```
Exec=spotify-tray
```

to

```
Exec=spotify-tray --hide-window
```

## Disclaimer

This software is in no way official and has nothing to do with the Spotify Technology S.A. company. Its only purpose is to add functionality missing in the official client.

Spotify is a registered trademark of Spotify AB.

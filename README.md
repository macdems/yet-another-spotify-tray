# Yet Another Spotify Tray

This application should not exist. Spotify should have this feature automatically.
This application  adds a tray icon to the Spotify Linux client application.

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
* CMake

## Disclaimer

This software is in no way official and has nothing to do with the Spotify Technology S.A. company. Its only purpose is to add functionality missing in the official client.

Spotify is a registered trademark of Spotify AB.

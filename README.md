# RebgilPlayer

某ゲームのアレの場面再生用。

## Runtime requirement

- Windows OS later than Windows 10
- MSVC 2015-2022 (x64)

## How to play

First, prepare all the necessary files, commented below, with proper directory.

<pre>
assets
  ├ bundles
  │  ├ ...
  │  ├ loaddata
  │  │  ├ ...
  │  │  ├ hcg // scene spine folder
  │  │  │  ├ ...
  │  │  │  ├ 203502
  │  │  │  │  ├ 2035_CG2.atlas.txt
  │  │  │  │  ├ 2035_CG2.png
  │  │  │  │  ├ 2035_CG2.skel.txt
  │  │  │  │  ├ 2035_CG2_2.png
  │  │  │  │  ├ 2035_CG2_3.png
  │  │  │  │  └ ...
  │  │  │  └ ...
  │  │  └ ...
  │  └ ...
  ├ ...
  ├ dialogvoice
  │  ├ ...
  │  ├ 203562 // scene voice folder
  │  │  ├ 203562010.m4a
  │  │  ├ 203562014.m4a
  │  │  └ ...
  │  └ ...
  ├ dialogxml // scene dialogue folder
  │  └ jp
  │     ├ ...
  │     ├ 203562
  │     │  ├ dialog_show.txt
  │     │  └ dialog_text.txt
  │     └ ...
  └ ...
</pre>

Then, select any of spine folder in `assets\bundles\loaddata\hcg\` from the application.  
The scene will be set up based on `dialog_show.txt` and `dialog_text.txt`.

## Mouse functions

| Input  | Function  |
| --- | --- |
| Mouse wheel | Scale up/down. Combinating with left `Ctrl` to retain window size. |
| Left button + mouse wheel | Speed up/down the animation. |
| Left button click | Switch to the next animation. |
| Left button drag | Move view-point. |
| Middle button | Reset scale, speed, and view-point to default. |
| Right button + mouse wheel | Show the next/previous text. |
| Right button + left button | Move window. |

## Keyboard functions

| Input  | Function  |
| --- | --- |
| A | Enable/disable premultiplied alpha. |
| B | Prefer/ignore blend-mode specified by slots. |
| C | Switch text colour between black and white. |
| T | Show/hide text. |
| Esc | Close the application. |
| Up | Move on to the next folder. |
| Down | Move on to the previous folder. |
| PageUp | Speed up the audio playback rate. |
| PageDown | Speed down the audio playback rate. |
| Home | Reset the audio playback rate.|  

- Some scenes require ignoring blend-mode specified by slots.

## Scene preferences

The following preferences can be configured through `setting.txt` in the same directory of the executable file.
- Font file with which the scene texts will be drawn.
- Text to be shown; those in `dialog_show.txt` or in `dialog_text.txt`. 

## External libraries

- [SFML-2.6.2](https://www.sfml-dev.org/download/sfml/2.6.2/)
- [spine-cpp-4.1](https://github.com/EsotericSoftware/spine-runtimes/tree/4.1)

## Build

1. Run `deps/CMakeLists.txt` to obtain the external libraries stated above. 
2. Open `RebgilPlayer.sln` with Visual Studio 2022.
3. Select `Build Solution` on menu items.

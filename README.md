# RebgilPlayer

某寝室用。

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
  │  │  ├ hcg // Scene spine folder
  │  │  │  ├ ...
  │  │  │  ├ 203502
  │  │  │  │  ├ 2035_CG2.atlas
  │  │  │  │  ├ 2035_CG2.png
  │  │  │  │  ├ 2035_CG2.skel
  │  │  │  │  ├ 2035_CG2_2.png
  │  │  │  │  ├ 2035_CG2_3.png
  │  │  │  │  └ ...
  │  │  │  ├ ...
  │  │  │  └ jp
  │  │  │    ├ ...
  │  │  │    ├ 200701
  │  │  │    │  ├ 2007_CG1.atlas
  │  │  │    │  └ ...
  │  │  │    └ ...
  │  │  └ ...
  │  └ ...
  ├ ...
  ├ dialogvoice
  │  ├ ...
  │  ├ 203562 // Scene voice folder
  │  │  ├ 203562010.m4a
  │  │  ├ 203562014.m4a
  │  │  └ ...
  │  └ ...
  ├ dialogxml // Scene dialogue folder
  │  └ jp
  │     ├ ...
  │     ├ 203562
  │     │  ├ dialog_show.xml
  │     │  └ dialog_text.xml
  │     └ ...
  └ ...
</pre>

Then, select `bundles/loaddata/hcg/XXXXXX`, or `bundles/loaddata/hcg/jp/XXXXXX` folder from the application.  
The scene will be set up based on `dialog_show.xml` and `dialog_text.xml`.

## Mouse functions

| Input | Function |
| --- | --- |
| Mouse wheel | Scale up/down. |
| Left button + mouse wheel | Speed up/down the animation. |
| Left button click | Play the next animation. |
| Left button drag | Move view-point. |
| Middle button | Reset scale, speed, and view-point to default. |
| Right button + mouse wheel | Fast forward/rewind the text. |
| Right button + left button | Move window. |

## Keyboard functions

| Input | Function |
| --- | --- |
| <kbd>A</kbd> | Enable/disable premultiplied alpha. _Default: disabled_|
| <kbd>B</kbd> | Prefer/ignore blend-mode specified by slots. _Default: ignored_|
| <kbd>C</kbd> | Toggle text colour between black and white. |
| <kbd>T</kbd> | Show/hide text. |
| <kbd>Esc</kbd> | Close the application. |
| <kbd>∧</kbd> | Open the next folder. |
| <kbd>∨</kbd> | Open the previous folder. |
| <kbd>＞</kbd> | Fast-forward the text. |
| <kbd>＜</kbd> | Rewind the text. |

- Some scenes require ignoring blend-mode specified by slots.

## Scene preferences

The following preferences can be configured through `setting.txt` in the same directory of the executable file.
- Font file with which the scene texts will be drawn.
- Text to be shown; those in `dialog_show.xml` or in `dialog_text.xml`. 

## External libraries

- [SFML-2.6.2](https://www.sfml-dev.org/download/sfml/2.6.2/)
- [spine-cpp-4.1](https://github.com/EsotericSoftware/spine-runtimes/tree/4.1)

## Build

1. Run `deps/CMakeLists.txt` to obtain the external libraries. 
2. Open `RebgilPlayer.sln` with Visual Studio 2022.
3. Select `Build Solution` on menu items.

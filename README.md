# xdg-desktop-portal-plus-filechooser

### Note that filepicking was mainly tested with lf file manager. I can't attest that others work properly!

[xdg-desktop-portal] backend for wlroots. A slightly modified version that integrates the wlroots functionality of screen recording and [GermainZ](https://github.com/GermainZ)'s [xdg-desktop-portal-termfilechooser](https://github.com/GermainZ/xdg-desktop-portal-termfilechooser) with some patches of [Boydaihungst](https://github.com/boydaihungst/xdg-desktop-portal-termfilechooser)'s fork, so credits go mainly to them. My contributions include merging two codebases and adding better handling for lf file manager. I would like it one day to be merged into mainline as from my testing everything seems to work fine but I'm afraid if it were to be deployed at a large scale.

# Filechooser configuration
```mkdir ~/.config/xdg-desktop-portal-wlr```
and create a config file (in the contrib folder there is a sample one, remember to change the name)
there you can adjust what filepicker script to execute (along with regular xdg-desktop-portal-wlr config options related to screenrecording). Choose the script corresponding to your file picker of choice and place it in corresponding location (must use absolute full path). (lf wrapper has extra configuration that needs to be done). After that continue with the [running guide](#Running).

## Building

```sh
meson build
ninja -C build
```

The fork also includes a PKGBUILD file for arch users, so you can just run ```makepkg``` inside the folder.

## Installing

### From Source

```sh
ninja -C build install
```

## Running

Make sure `XDG_CURRENT_DESKTOP` is set. Make sure `WAYLAND_DISPLAY` and
`XDG_CURRENT_DESKTOP` are imported into D-Bus. If you're running Sway, this
can be added to your config file:

    exec dbus-update-activation-environment --systemd WAYLAND_DISPLAY XDG_CURRENT_DESKTOP=sway

When correctly installed, xdg-desktop-portal should automatically invoke
xdg-desktop-portal-wlr when needed.

### Configuration

See `man 5 xdg-desktop-portal-wlr`.

### Manual startup

At the moment, some command line flags are available for development and
testing. If you need to use one of these flags, you can start an instance of
xdpw using the following command:

```sh
xdg-desktop-portal-wlr -r [OPTION...]
```

To list the available options, you can run `xdg-desktop-portal-wlr --help`.

## FAQ

Check out or [FAQ] for answers to commonly asked questions.

Please see the [screencast compatibility] guide for more information on
compatible applications and how to get them working.

If you have a question or problem that is not mentioned in those documents,
please open an issue or come chat with us in #sway on Libera Chat.

## Contributing

If you're interested in testing or development, check out
[CONTRIBUTING.md] for more information.

## License

MIT

[xdg-desktop-portal]: https://github.com/flatpak/xdg-desktop-portal
[FAQ]: https://github.com/emersion/xdg-desktop-portal-wlr/wiki/FAQ
[screencast compatibility]: https://github.com/emersion/xdg-desktop-portal-wlr/wiki/Screencast-Compatibility
[CONTRIBUTING.md]: CONTRIBUTING.md

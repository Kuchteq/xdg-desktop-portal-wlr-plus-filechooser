# Maintainer: Christian Rebischke <chris.rebischke@archlinux.org>
# Maintainer: Maxim Baz <$pkgname at maximbaz dot com>
# Contributor: Stefan Tatschner <stefan@rumpelsepp.org>

pkgname=xdg-desktop-portal-wlr
pkgver=0.7.0
pkgrel=1
pkgdesc='xdg-desktop-portal backend for wlroots'
url="https://github.com/emersion/xdg-desktop-portal-wlr"
arch=('x86_64')
license=('MIT')
provides=('xdg-desktop-portal-impl')
depends=('xdg-desktop-portal' 'pipewire' 'pipewire-session-manager' 'libinih')
makedepends=('meson' 'wayland-protocols' 'wayland' 'scdoc')
optdepends=(
    'slurp: to choose which output to screencast using slurp'
    'wofi: to choose which output to screencast using wofi'
    'bemenu: to choose which output to screencast using bemenu'
)

build() {
    cd ../
    arch-meson -Dsd-bus-provider=libsystemd build
    ninja -C build
}

package() {
    cd ../
    DESTDIR="${pkgdir}" ninja -C build install
    install -Dm644 -t "$pkgdir/usr/share/licenses/${pkgname}" LICENSE
}

# Pond landing page

Phoenix serves this page at `/`. HTML, CSS, JavaScript, fonts, and artwork are local static files; there is no frontend build or third-party browser request. The landing route and its assets sit before the API throttle. The existing settings page stays at `/settings.html`.

The plop button is an illustration only: it switches between actual emulator screenshots and animates the stone and rings without contacting the API. Pause motion stops decoration and uses an immediate demo transition. The operating system's reduced-motion preference takes precedence. With JavaScript disabled, the story and Still watch screenshot remain visible.

`stone.svg` and `icon.png` derive from the approved Pond brand assets in `design/store/`. The watch images are the original Basalt captures from the same pack, displayed with pixelated scaling. Refresh these copies when the watch artwork changes. The watch surround and oversized pond are illustrations.

`fraunces.ttf` is the Fraunces variable font from [Google Fonts](https://github.com/google/fonts/tree/main/ofl/fraunces), included under the accompanying SIL Open Font License in `OFL.txt`. Body text uses system fonts. No font service is contacted by visitors.

The page intentionally points to the source repository while public release and physical-device testing are pending. Add a store link when a real listing exists.

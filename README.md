# Pond

A little shared pond for people wearing Pebble watches.

Drop a pebble. Somewhere, another wearer may feel a ripple. Nothing needs answering.

Pond is a free, open-source community gift by Michaël / Monocursive. This repository currently contains product specifications and UI/UX designs; the application and service have not been implemented or deployed.

- [First release](docs/FIRST-RELEASE.md) — the bounded implementation scope and release gates.
- [North star](docs/NORTH-STAR.md) — the longer-term vision, revised around wearer comfort.
- [Original v0.2](docs/archive/PRODUCT-SPEC-v0.2.md) — the original proposal, preserved verbatim for context; it is not an implementation contract.
- [UI/UX design](design/DESIGN-HANDOFF.md) — editable Figma screens, native display layouts, core prototypes, and exported artwork.

The chosen backend is **Elixir / Phoenix**, with a **C watchface** and **PebbleKit JS** bridge. The first-release specification takes precedence over the north star for implementation. Later ideas are possibilities, not promised features.

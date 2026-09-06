# LVX-RUN-PLUGIN

Demo LVX plugin repository (topic: `lvx-plugin`).

Consumed by `lvx init` (software-source mechanism): each plugin lives in the
default-branch directory `lvx-plugin/` with a `manifest.json` describing the
artifacts (`{file, sha256, size}`), which are downloaded and sha256-verified.

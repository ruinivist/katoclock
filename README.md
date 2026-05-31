# katoclock

Just a QML-only cat clock widget for KDE Plasma 6. Bootstrapped from my [kde-6-widget-starter](https://github.com/ruinivist/kde-6-widget-starter)

[Pling page](https://www.pling.com/p/2349620/)

![Screenshot from desktop](Example.png)

# Install

- clone the repo
- `make install` from repo root

# Package

- `make package` builds a `.plasmoid` archive from the QML package and bundled image assets

# Pling Upload

This repo includes the same OpenDesktop/Pling uploader flow used by
`kde-6-widget-starter`.

Dry run validates login, project edit-page access, and upload endpoint discovery:

```bash
uv run python scripts/pling_upload.py --dry-run
```

Live mode uploads the packaged plasmoid and replaces the current files on the
target Pling project:

```bash
uv run python scripts/pling_upload.py -f build/katoclock.ruiny.de.plasmoid
```

The uploader reads the same environment variables as the starter:
`PLING_PROJECT_ID`, `PLING_USERNAME`, `PLING_PASSWORD`, `PLING_FILES`,
`PLING_TIMEOUT`, and `PLING_MAX_RETRIES`.

# GitHub Actions Release Upload

The workflow lives at `.github/workflows/pling-release-upload.yml`.

- Trigger: push a git tag matching `release*`, for example `release-v1.0.1`
- Build artifact: `build/katoclock.ruiny.de.plasmoid`
- Upload target: the Pling project identified by `PLING_PROJECT_ID`

Required GitHub repository secrets:

- `PLING_PROJECT_ID`
- `PLING_USERNAME`
- `PLING_PASSWORD`

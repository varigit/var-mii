#!/usr/bin/env bash

set -euo pipefail

DIST="${1:-bookworm}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BASE_VERSION="${PKG_VERSION_PREFIX:-1.0}"
cd "${REPO_ROOT}"

if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    # Only consider tags that look like version strings (vX.Y.Z). If none exist,
    # fall back to BASE_VERSION to keep the package version stable.
    if tag=$(git describe --tags --match 'v[0-9]*' --abbrev=0 2>/dev/null); then
        version_prefix="${tag#v}"
    else
        version_prefix="${BASE_VERSION}"
    fi
    commit="$(git rev-parse HEAD)"
    short_commit="$(git rev-parse --short=12 HEAD)"
else
    version_prefix="${BASE_VERSION}"
    # Prefer the GitHub-provided SHA when the working tree is a source export.
    commit="${PKG_COMMIT:-${GITHUB_SHA:-}}"
    if [[ -z "${commit}" ]]; then
        echo "Error: Unable to determine commit hash. Run inside a git repository or set PKG_COMMIT/GITHUB_SHA." >&2
        exit 1
    fi
    short_commit="${commit:0:12}"
fi

version_prefix="${version_prefix#v}"
timestamp="$(date -u +%Y%m%d)"
deb_version="${version_prefix}+git${timestamp}.${short_commit}-1"

cat > debian/changelog <<EOF
var-mii (${deb_version}) ${DIST}; urgency=medium

  * Automated build from ${commit}

 -- Nate Drude <nate.d@variscite.com>  $(date -u +"%a, %d %b %Y %H:%M:%S +0000")
EOF

cat <<INFO
Updated debian/changelog with:
  Version : ${deb_version}
  Distro  : ${DIST}
  Commit  : ${commit}
INFO

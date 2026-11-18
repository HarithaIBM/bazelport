# Vendored pristine upstream tarballs

Offline fallback copies of the upstream rule-repo tarballs that
`buildenv` provisioning downloads. The **pinned URLs in `buildenv`
(`_dlrepo` calls) are the source of truth**; these copies are used only
when GitHub is unreachable (a one-shot connectivity probe decides).

## When / how to refresh

Whenever you bump a `_dlrepo` URL in `buildenv` (new upstream version),
re-vendor the matching tarball **and** its checksum:

```sh
cd deps/pristine
curl -sSL -o <name>.tar.gz "<the new URL from buildenv>"
cksum <name>.tar.gz >> SHA256SUMS   # or: sha256sum, whatever zopen has
```

Commit both. The checksum file lets anyone verify a vendored copy still
matches what the pinned URL serves.

## Current contents

| tarball | upstream | pinned version |
|---|---|---|
| bazel_skylib.tar.gz | bazelbuild/bazel-skylib | 1.6.1 |
| rules_license.tar.gz | bazelbuild/rules_license | 0.0.7 |
| rules_pkg.tar.gz | bazelbuild/rules_pkg | 0.9.1 |
| rules_jvm_external.tar.gz | bazelbuild/rules_jvm_external | 6.0 |
| blake3.tar.gz | BLAKE3-team/BLAKE3 | 1.5.1 |
| stardoc.tar.gz | bazelbuild/stardoc | 0.5.6 |

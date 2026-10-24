#!/usr/bin/env python3
"""Export an exclusive, text-only support snapshot; never upload or build.

Reason: share the port sources without firmware, raw logs, keys or device IDs.
Files: a new destination directory only; originals remain unchanged.
Verify: explicit allowlist, known-device and local absolute-path redaction,
secret-pattern rejection, and SHA256 manifest. Patch replay must be checked
separately before publishing.
"""
import argparse
import hashlib
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SUFFIXES = {'.md', '.json', '.tsv', '.py', '.sh', '.c', '.patch', '.txt'}
SECRET = re.compile(
    r'gh[pousr]_[A-Za-z0-9]{20,}|github_pat_[A-Za-z0-9_]{20,}|'
    r'-----BEGIN (?:RSA |OPENSSH |EC )?PRIVATE KEY-----|'
    r'https?://[^\s/@]+:[^\s/@]+@'
)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('destination', type=Path)
    args = parser.parse_args()
    dest = args.destination.resolve()
    dest.mkdir(parents=True, exist_ok=False)
    aliases = {
        'M2_G965F_SERIAL': 'M2_G965F_SERIAL',
        'M1_G965F_SERIAL': 'M1_G965F_SERIAL',
        'G965N_SERIAL': 'G965N_SERIAL',
    }
    files = [ROOT / name for name in (
        '.gitignore', 'AGENTS.md', 'README.md', 'STATUS.md',
        'sources.lock.json', 'firmware.lock.json', 'tools.lock.json')]
    for directory in ('docs', 'patches', 'diagnostics', 'tools', 'support'):
        for path in sorted((ROOT / directory).rglob('*')):
            if path.is_file() and '__pycache__' not in path.parts:
                if path.suffix not in SUFFIXES and path.name != 'series':
                    raise ValueError(f'Unapproved source suffix: {path.relative_to(ROOT)}')
                files.append(path)
    manifest = {}
    redactions = 0
    for source in files:
        if source.is_symlink() or source.stat().st_size > 5 * 1024 * 1024:
            raise ValueError(f'Unexpected symlink/large source: {source}')
        text = source.read_text(encoding='utf-8')
        if '\x00' in text or SECRET.search(text):
            raise ValueError(f'Binary or secret pattern detected in {source.relative_to(ROOT)}')
        for identifier, alias in aliases.items():
            redactions += text.count(identifier)
            text = text.replace(identifier, alias)
        text = text.replace(str(ROOT), '${PROJECT_ROOT}')
        # Do not publish the workstation username, local Downloads names, or
        # other machine-specific absolute paths from source/firmware manifests.
        text = re.sub(r'/(?:home|Users)/[^\s"\'`<>]+', '${LOCAL_ABSOLUTE_PATH}', text)
        relative = source.relative_to(ROOT)
        target = dest / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(text, encoding='utf-8')
        target.chmod(0o755 if source.suffix == '.sh' else 0o644)
        manifest[str(relative)] = {
            'bytes': target.stat().st_size,
            'sha256': hashlib.sha256(target.read_bytes()).hexdigest(),
        }
    # Redaction can change diagnostic patch metadata. Keep exported lock hashes
    # usable, while recording original local hashes for provenance.
    lock_path = dest / 'sources.lock.json'
    lock = json.loads(lock_path.read_text())
    for patch in lock['fork']['local_patches']:
        actual = hashlib.sha256((dest / patch['path']).read_bytes()).hexdigest()
        if patch['sha256'] != actual:
            patch['local_sha256_before_identifier_redaction'] = patch['sha256']
            patch['sha256'] = actual
    lock_path.write_text(json.dumps(lock, indent=2, ensure_ascii=False) + '\n')
    manifest['sources.lock.json'] = {
        'bytes': lock_path.stat().st_size,
        'sha256': hashlib.sha256(lock_path.read_bytes()).hexdigest(),
    }
    metadata = {
        'date': '2026-10-24',
        'purpose': 'Source and selected diagnostic evidence for external help',
        'fork_url': 'https://github.com/Eend15/Unofficial-Un1ca-9810',
        'fork_commit': '2b25ae6fc3098650be249bd1898e21e563cd5df1',
        'active_patches': len([p for p in (ROOT/'patches/fork/series').read_text().splitlines()
                               if p and not p.startswith('#')]),
        'device_identifier_replacements': redactions,
        'binary_firmware_raw_logs_credentials_included': False,
        'device_serials_in_exported_text_are_role_aliases': True,
        'files': manifest,
    }
    (dest / 'SOURCE_SNAPSHOT.json').write_text(json.dumps(metadata, indent=2) + '\n')
    print(json.dumps({'destination': str(dest), 'files': len(manifest),
                      'bytes': sum(v['bytes'] for v in manifest.values()),
                      'identifier_replacements': redactions}))


if __name__ == '__main__':
    main()

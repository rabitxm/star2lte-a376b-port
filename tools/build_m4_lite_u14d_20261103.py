#!/usr/bin/env python3
"""Create a separate M4 Lite ZIP32 with a hash-verified G965F uptime kernel."""
import hashlib, io, os, shutil, subprocess, tempfile, zipfile
from pathlib import Path, PurePosixPath
import bsdiff4
root=Path(__file__).resolve().parent.parent
rom=root/'rom'
source=rom/'out/releases/M4/star2lte-preformatted-1793692940-kKZ7vE/M4-Lite-Preformatted-Internal-ZIP32-sign.zip'
outdir=rom/'out/releases/M4/star2lte-uptime14d-20261103'
scratch=Path('${LOCAL_ABSOLUTE_PATH}')
expected='fa600a7395eaddf5e5f2cd782cbd28b755ba83f09f7de3fa5e72c55947ae614c'
baseline='92b14be170225954476869d1fd7700f9c2c92d98eb6447d0181c50a11e80fcdd'
def digest_file(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(8*1024*1024),b''): h.update(b)
 return h.hexdigest()
def digest_stream(f):
 h=hashlib.sha256()
 for b in iter(lambda:f.read(8*1024*1024),b''): h.update(b)
 return h.hexdigest()
if (scratch/'build.exit').read_text().strip()!='0': raise SystemExit('Kernel build not successful')
config=(scratch/'out/.config').read_text(); assert 'CONFIG_PORTKIT_BOOT_UPTIME_OFFSET_SECONDS=1209600\n' in config
assert 'CONFIG_KSU=y\n' not in config and 'CONFIG_ALWAYS_PERMISSIVE=y\n' not in config
if digest_file(source)!=expected: raise SystemExit('Unexpected source ZIP SHA-256')
image=(scratch/'out/arch/arm64/boot/Image').read_bytes(); image_hash=hashlib.sha256(image).hexdigest()
if outdir.exists(): raise SystemExit('Refusing to overwrite existing release')
outdir.mkdir()
with tempfile.TemporaryDirectory(prefix='m4-u14d-',dir=outdir) as t:
 tmp=Path(t); payload=tmp/'payload'; payload.mkdir()
 with zipfile.ZipFile(source) as z:
  names=z.namelist(); assert len(names)==len(set(names))
  for n in names:
   p=PurePosixPath(n)
   if p.is_absolute() or '..' in p.parts or '\\' in n: raise SystemExit('Unsafe ZIP entry')
  nested=z.read('exynos9810/kernel.zip')
 with zipfile.ZipFile(io.BytesIO(nested)) as k:
  base=k.read('floyd/G960F-kernel'); old=bsdiff4.patch(base,k.read('floyd/G965F-kernel'))
  if hashlib.sha256(old).hexdigest()!=baseline: raise SystemExit('Unexpected old G965F kernel')
  delta=bsdiff4.diff(base,image)
  if bsdiff4.patch(base,delta)!=image: raise SystemExit('New kernel delta does not reconstruct')
  target=io.BytesIO()
  with zipfile.ZipFile(target,'w',allowZip64=False) as out:
   for item in k.infolist(): out.writestr(item,delta if item.filename=='floyd/G965F-kernel' else k.read(item))
 subprocess.run(['7z','x','-y',str(source),'-o'+str(payload)],check=True,stdout=subprocess.DEVNULL)
 (payload/'exynos9810/kernel.zip').write_bytes(target.getvalue())
 kernel_name=payload/'exynos9810/kernel_name'
 original_name=kernel_name.read_text(); kernel_name.write_text(original_name.rstrip()+' + PortKit uptime 14d (experimental)\n')
 unsigned=tmp/'unsigned.zip'; signed=tmp/'signed.zip'
 large=sorted(p.name for p in payload.iterdir() if p.name.endswith(('.new.dat.br','.patch.dat')))
 if not large: raise SystemExit('Missing image payload')
 subprocess.run(['7z','a','-tzip','-mx=0',str(unsigned),*large],cwd=payload,check=True,stdout=subprocess.DEVNULL)
 subprocess.run(['7z','a','-tzip','-mx=3',str(unsigned),'-r','*','-x!*.new.dat.br','-x!*.patch.dat'],cwd=payload,check=True,stdout=subprocess.DEVNULL)
 env=os.environ.copy(); env['SRC_DIR']=str(rom); env['ROM_IS_OFFICIAL']='false'; env['PATH']=str(rom/'out/tools/bin')+':'+env['PATH']
 subprocess.run([str(rom/'out/tools/bin/signapk'),'-w',str(rom/'security/aosp_testkey.x509.pem'),str(rom/'security/aosp_testkey.pk8'),str(unsigned),str(signed)],env=env,check=True)
 subprocess.run(['python3',str(rom/'scripts/internal/verify_m4_lite_zip32.py'),str(signed)],env=env,check=True)
 with zipfile.ZipFile(source) as a, zipfile.ZipFile(signed) as b:
  oldnames={i.filename for i in a.infolist() if not i.is_dir()}; newnames={i.filename for i in b.infolist() if not i.is_dir()}
  if oldnames!=newnames: raise SystemExit('ZIP entry set changed')
  for n in sorted(oldnames-{'exynos9810/kernel.zip','exynos9810/kernel_name'}):
   with a.open(n) as x,b.open(n) as y:
    if digest_stream(x)!=digest_stream(y): raise SystemExit('Changed unintended entry: '+n)
  k=zipfile.ZipFile(io.BytesIO(b.read('exynos9810/kernel.zip')))
  if hashlib.sha256(bsdiff4.patch(k.read('floyd/G960F-kernel'),k.read('floyd/G965F-kernel'))).hexdigest()!=image_hash: raise SystemExit('Packaged kernel mismatch')
 final=outdir/'M4-Lite-Preformatted-G965F-Uptime14d-ZIP32-sign.zip'; os.link(signed,final); final.chmod(0o444)
 (outdir/'SHA256SUMS').write_text(digest_file(final)+'  '+final.name+'\n')
 (outdir/'PROVENANCE.txt').write_text('source_sha256='+expected+'\nbaseline_image_sha256='+baseline+'\nnew_image_sha256='+image_hash+'\nuptime_offset_seconds=1209600\nfirst_boot=UNTESTED\n')
 (outdir/'.complete').touch()
 print('PACKAGE_PASS', final, digest_file(final), flush=True)

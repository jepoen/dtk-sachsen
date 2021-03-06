import argparse, json, os, subprocess
from PIL import Image
from pyproj import Proj

class Converter:
  def __init__(self):
    self.proj = Proj("+proj=utm +zone=33")

  def fromUtm(self, r, t):
    return self.proj(r, t, inverse=True)

def convert(args):
  tileType = args['type']
  if args['name'] is None:
    args['name'] = tileType
  outDir = os.path.join(args['output_dir'], tileType)
  os.makedirs(outDir, exist_ok=True)
  utm = Converter()
  xtiles = dict()
  ytiles = dict()
  tiles = dict()
  for d in os.listdir(args['input_dir']):
    if not os.path.isdir(d): continue
    for fileName in os.listdir(d):
      parts = fileName.split('_')
      if len(parts) != 4: continue
      tx, ty = parts[1:3]
      tile, ext = parts[3].split('.')
      if ext != 'tif': continue
      if tile != tileType: continue
      key = '_'.join(parts[1:3])
      tgtName = key+'.png'
      tgtPath = os.path.join(outDir, tgtName)
      print(os.path.join(d, fileName), tgtPath)
      subprocess.run(['convert', os.path.join(d, fileName), tgtPath])
      dataFile = '_'.join(parts[:-1])+'_'+tileType+'.tfw'
      #print(dataFile)
      im = Image.open(os.path.join(d, fileName))
      w, h = im.size
      im.close()
      with open(os.path.join(d, dataFile)) as fi:
        a = float(fi.readline())
        fi.readline() # skip
        fi.readline() # skip
        d = float(fi.readline())
        left = float(fi.readline())
        top = float(fi.readline())
        right = left + a*w
        bottom = top + d*h
        tile = {
          'png': tgtName,
          'size': [w, h],
          'utm': [[left, top], [right, bottom]],
          'wgs': [utm.fromUtm(left, top), utm.fromUtm(right, bottom)]
        }
        xtiles[tx] = {
          'x': tx,
          'utm0': tile['utm'][0][0],
          'utm1': tile['utm'][1][0],
          'lon0': tile['wgs'][0][0],
          'lon1': tile['wgs'][1][0],
        }
        ytiles[ty] = { # upper border
          'y': ty,
          'utm0': tile['utm'][0][1],
          'utm1': tile['utm'][1][1],
          'lat0': tile['wgs'][0][1],
          'lat1': tile['wgs'][1][1],
        }
        tiles[key] = tile
        #print(a, d, left, top, utm.fromUtm(left, top))
        #print(a, d, right, bottom, utm.fromUtm(right, bottom))
  xlist = list()
  for x in sorted(xtiles):
    xlist.append(xtiles[x])
  ylist = list()
  for y in sorted(ytiles):
    ylist.append(ytiles[y])
  res = {
    'name': args['name'],
    'xtiles': xlist,
    'ytiles': ylist,
    'tiles': tiles,
  }
  with open(os.path.join(outDir, 'coords.json'), 'w') as fo:
    json.dump(res, fo, indent=2)

def run():
  parser = argparse.ArgumentParser(
    description='Extract and convert DTK map tiles')
  parser.add_argument('--output_dir', '-o',
    help='base output dir (type is appended)', default=".")
  parser.add_argument('--input_dir', '-i',
    help='base input dir', default=".")
  parser.add_argument('--type', '-t',
    help='Tile type (part of filename: col, bcgr, grbr, ...)', default='col')
  parser.add_argument('--name', '-n',
    help='Map name')
  args = parser.parse_args()
  print(vars(args))
  convert(vars(args))
if __name__ == '__main__':
  run()

# Test
pr = Converter()
lon, lat = pr.fromUtm(316000,5701999)
print(lon, lat)

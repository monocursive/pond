"""Convert the checked-in Figma SVG to integer Pebble paths (stdlib only).

Run manually after changing pebble.svg; normal `pebble build` needs no generator.
Only the absolute M/L/C/V/Z commands used by this exported artwork are accepted.
"""
from pathlib import Path
import re
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parent
PAINTS = {'#70B9A6': 0, '#FFF1C7': 1, '#E4B96F': 2,
          '#BD864B': 3, '#163F39': 4}
paths = []
for node in ET.parse(ROOT / 'pebble.svg').iter('{http://www.w3.org/2000/svg}path'):
    tokens = re.findall(r'[A-Za-z]|-?\d+(?:\.\d+)?', node.attrib['d'])
    points = []
    position = (0, 0)
    index = 0
    command = None

    def finish(closed=False):
        if not points:
            return
        # Discard adjacent points which round to the same native SVG coordinate.
        rounded = []
        for point in points:
            point = tuple(round(v) for v in point)
            if not rounded or point != rounded[-1]:
                rounded.append(point)
        assert len(rounded) <= 64
        paths.append((rounded, PAINTS[node.attrib.get('fill', node.attrib.get('stroke'))],
                      round(float(node.attrib.get('stroke-width', '0'))), closed,
                      node.attrib['id']))

    while index < len(tokens):
        if tokens[index].isalpha():
            command = tokens[index]
            index += 1
        assert command in ('M', 'L', 'C', 'V', 'Z'), command
        if command == 'Z':
            finish(True)
            points = []
            command = None
            continue
        count = {'M': 2, 'L': 2, 'C': 6, 'V': 1}[command]
        values = [float(v) for v in tokens[index:index + count]]
        index += count
        if command == 'M':
            finish()
            points = []
            position = tuple(values)
            points.append(position)
            command = 'L'
        elif command == 'L':
            position = tuple(values)
            points.append(position)
        elif command == 'V':
            position = (position[0], values[0])
            points.append(position)
        else:
            start = position
            for step in range(1, 9):
                t = step / 8
                points.append(tuple((1-t)**3*start[a] + 3*(1-t)**2*t*values[a]
                                    + 3*(1-t)*t*t*values[a+2] + t**3*values[a+4]
                                    for a in range(2)))
            position = tuple(values[4:])
    finish()

lines = ['/* Generated from resources/pebble.svg by resources/convert-art.py. */',
         '#pragma once', '#include <pebble.h>',
         'typedef struct { const GPoint *points; uint8_t count, paint, stroke; bool closed; } PondPath;']
for i, (points, *_rest) in enumerate(paths):
    lines.append('static const GPoint pond_points_%d[] = {%s};' %
                 (i, ','.join('{%d,%d}' % point for point in points)))
lines.append('static const PondPath pond_paths[] = {')
for i, (points, paint, stroke, closed, name) in enumerate(paths):
    lines.append('  {pond_points_%d, %d, %d, %d, %s}, /* %s */' %
                 (i, len(points), paint, stroke, str(closed).lower(), name))
lines.append('};')
(ROOT.parent / 'src/c/art-data.h').write_text('\n'.join(lines) + '\n')
print(f'Converted {len(paths)} paths from the Figma source.')

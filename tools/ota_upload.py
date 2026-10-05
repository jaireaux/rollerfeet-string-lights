#!/usr/bin/env python3
"""Upload using the installed ESP32 core, reading the password privately."""
import argparse
import importlib.util
import json
import logging
from pathlib import Path
import re
import sys

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--ip', required=True)
    parser.add_argument('--file', required=True, type=Path)
    parser.add_argument('--espota', type=Path)
    parser.add_argument('--host-ip', default='0.0.0.0')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    config = root / 'XIAO_ESP32_holiday_lights' / 'ota_secrets.h'
    if not config.exists():
        parser.error('Configure private ota_secrets.h first.')
    match = re.search(r'^\s*#define\s+SECRET_OTA_PASSWORD\s+("[^"\n]+")',
                      config.read_text(), re.MULTILINE)
    if not match:
        parser.error('OTA password must be a quoted string in the private file.')
    password = json.loads(match.group(1))
    if len(password) < 12:
        parser.error('OTA password must contain at least 12 characters.')
    if not args.file.is_file():
        parser.error('Compiled application .bin not found.')
    uploader = args.espota
    if uploader is None:
        cores = Path.home() / 'Library/Arduino15/packages/esp32/hardware/esp32'
        versions = [p for p in cores.glob('*/tools/espota.py')
                    if re.fullmatch(r'\d+\.\d+\.\d+', p.parents[1].name)]
        if not versions:
            parser.error('Pass --espota pointing to the installed ESP32 core uploader.')
        uploader = max(versions, key=lambda p: tuple(map(int, p.parents[1].name.split('.'))))
    spec = importlib.util.spec_from_file_location('espota', uploader)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    module.TIMEOUT = 10
    module.PROGRESS = False
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    # Password stays in memory, never on a subprocess command line.
    return module.serve(args.ip, args.host_ip, 3232, 3240, password, False, str(args.file))

if __name__ == '__main__':
    sys.exit(main())

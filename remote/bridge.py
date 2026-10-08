#!/usr/bin/env python3
"""Outbound-only IONOS relay. Standard-library Python; no AI or inbound listener."""
import base64
import json
import logging
import signal
import time
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

class NoRedirect(urllib.request.HTTPRedirectHandler):
    def redirect_request(self, *args, **kwargs):
        raise urllib.error.HTTPError(args[0].full_url, 403, 'Redirect refused', {}, None)

opener = urllib.request.build_opener(NoRedirect)

def request(url, authorization, payload=None, form=False):
    headers = {'Authorization': authorization, 'X-Holiday-Control': '1'}
    if payload is None:
        data = None
    elif form:
        data = urllib.parse.urlencode(payload).encode()
        headers['Content-Type'] = 'application/x-www-form-urlencoded'
    else:
        data = json.dumps(payload).encode()
        headers['Content-Type'] = 'application/json'
    req = urllib.request.Request(url, data=data, headers=headers)
    with opener.open(req, timeout=5) as response:
        return json.loads(response.read(8192))

def run(config):
    cloud = config['relay_url']
    parsed = urllib.parse.urlparse(cloud)
    if parsed.scheme != 'https' or parsed.hostname != 'rollerfeet.com':
        raise ValueError('Relay must use HTTPS on rollerfeet.com')
    controller = config['controller_url'].rstrip('/')
    controller_auth = 'Basic ' + base64.b64encode(('admin:' + config['controller_password']).encode()).decode()
    cloud_auth = 'Bearer ' + config['bridge_token']
    ack = None
    previous = None
    while True:
        started = time.monotonic()
        state = None
        try:
            state = request(controller + '/api/state', controller_auth)
        except (OSError, ValueError):
            pass
        try:
            result = request(cloud + '?action=bridge', cloud_auth, {'state': state, 'ack': ack})
            ack = None
            online = state is not None
            if online != previous:
                logging.info('Controller %s', 'connected' if online else 'unavailable')
                previous = online
            command = result.get('command')
            if command:
                # The cloud atomically claims commands. Do not retry a request with
                # an ambiguous response: "next" could otherwise advance twice.
                ok = False
                if command['action'] in ('power','brightness','animation','auto','playlist','duration','next'):
                    try:
                        request(controller + '/api/control', controller_auth,
                                {'action':command['action'],'value':command['value']}, form=True)
                        ok = True
                    except (OSError, ValueError):
                        pass
                ack = {'id':command['id'],'ok':ok}
                logging.info('Light command %s', 'confirmed' if ok else 'unconfirmed')
        except (OSError, ValueError):
            if previous != 'relay-unavailable':
                logging.warning('Website relay unavailable; will reconnect')
                previous = 'relay-unavailable'
        time.sleep(max(0.2, 2 - (time.monotonic() - started)))

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=Path, required=True)
    args = parser.parse_args()
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    signal.signal(signal.SIGTERM, lambda *_: exit(0))
    run(json.loads(args.config.read_text()))

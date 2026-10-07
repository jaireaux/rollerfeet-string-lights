# IONOS persistent-connection research

Checked 2026-10-07. This concerns the existing shared PHP/SFTP webspace, not all IONOS products. No hosting purchase, websocket server, MQTT broker, or direct-controller cloud agent was installed during this research.

## Findings

IONOS explicitly documents WebSocket support for its Cloud Managed Application Load Balancer. That is separate cloud infrastructure; it does not establish WebSocket support on this project's shared webspace. The shared-hosting PHP documentation describes script execution-time settings, not a supported long-running broker or WebSocket server. I found no explicit current official blanket prohibition, and no affirmative shared-webspace support guarantee. Treat persistent push support on the present plan as unverified; polling remains the demonstrated working route.

Official references:
- [Cloud ALB WebSocket configuration](https://docs.ionos.com/cloud/network-services/application-load-balancer/how-tos/configure-websocket)
- [Shared-hosting PHP settings](https://www.ionos.co.uk/help/hosting/using-php-for-web-projects/which-php-settings-can-i-change/)

## Requested community checks

A Reddit webdev discussion dated August 23, 2021 includes a user's report that their IONOS hosting did not allow installing Node.js, and a comment warning that a PHP WebSocket server needs a continuing CLI process. This is old, anecdotal evidence, not confirmation of the current plan. A Stack Overflow answer from 2019 describes the usual shared-hosting obstacles: permitted inbound ports or a web-server proxy that handles WebSocket upgrades. That answer is general shared-hosting guidance, not an IONOS support statement. A 2021 IONOS-tagged question has no useful plan-support confirmation.

- [Reddit: AJAX vs WebSocket](https://www.reddit.com/r/webdev/comments/p9veqt)
- [Stack Overflow: PHP WebSocket service on shared hosting](https://stackoverflow.com/questions/57989437/how-to-setup-and-run-a-php-websocket-service-on-a-cpanel-based-shared-hosting-pl)
- [Stack Overflow: WebSocket on IONOS hosting or VPS](https://stackoverflow.com/questions/66252998/how-to-host-my-websocket-on-ionos-hosting-or-vps)

## Project implication

Keep the current authenticated HTTPS polling relay for this hosting plan. Direct XIAO-to-IONOS polling is a possible subsequent implementation; it has not replaced LNM. A persistent outbound device connection would require a supported broker/WebSocket service, possibly on a separate hosting product. A database-only initial check-in provides discovery metadata but no inbound route to a controller behind a home router. Long polling still occupies finite server requests and must not be presented as an indefinitely persistent connection.

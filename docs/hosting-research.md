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

## Request and connection limits — checked 2026-10-07

IONOS's UK support article for performance-level hosting packages ordered through September 2025 labels its limits **PHP processes per minute**: level 1: 300; level 2: 600; level 3: 900; level 4: 1,200; level 5: 1,500. These apply across the package, not just this app. They are not a published universal HTTP-request quota or a count of simultaneously open TCP connections. The older-package article defines NPROC as concurrent PHP processes and lists 10–20 for pre-March-2017 web hosting, depending on package/generation, with script runtimes of 20–60 seconds. These are simultaneous processes, not requests per minute. The article distinguishes unchanged pre-March-2017 packages and newer packages; the current account's contract generation and level remain unverified. The article title says September 14 while its requirements say September 15, so confirm boundary contracts with support.

- [IONOS performance-level table and eligibility](https://www.ionos.co.uk/help/hosting/scaling-web-project-performance/web-hosting-performance-levels-packages-ordered-until-september-14-2025/)
- [Older PHP package limits](https://www.ionos.co.uk/help/hosting/troubleshooting-for-php/php-script-limits-in-older-ionos-web-hosting-packages-up-to-march-22-2017/)

The live lights endpoint returned `X-WS-RateLimit-Limit: 1000` and remaining `999` on a single read. Official Cloud CDN documentation associates this header with a per-client-IP/routing-rule/edge bucket and documents per-second classes 1–500. Because the observed value differs and the shared-webspace configuration is unknown, do not claim an account entitlement of 1,000 requests/second from that header alone. No verified current-plan daily/monthly request quota or maximum concurrent HTTP connection count was found.

- [Official CDN rate classes](https://docs.ionos.com/cloud/network-services/cdn/overview/features-benefits)
- [Official CDN header/bucket explanation](https://docs.ionos.com/cloud/network-services/cdn/cdn-faqs)

A December 2021 Reddit report describes throttling URL checks to roughly 12/second on low-tier IONOS hosting; that is an anecdotal workload observation, not a contractual limit. Stack Exchange searches found general shared-hosting/browser concurrency explanations but no verified numeric limit for this account.

- [Reddit: photo-hosting throughput report](https://www.reddit.com/r/webhosting/comments/rbuaev)
- [Webmasters Stack Exchange: concurrent Apache requests](https://webmasters.stackexchange.com/questions/26724/notification-for-too-many-apache-connection-errors-on-shared-hosting)

Current nominal relay traffic: one LNM report every 2 seconds = 30 requests/minute, 1,800/hour, 43,200/day. Each visible app polls every 3 seconds = another 20/minute, 1,200/hour, 28,800/day if continuously visible. Actual throughput is reduced by request execution time; control commands and initial page loads add traffic. One open app plus LNM is approximately 50/minute before other website traffic. This is below the lowest documented 300-process/minute tier if that tier applies, but request count does not establish resource usage or guarantee capacity.

## Live animation selection

On 2026-10-07, the authenticated remote playlist command was acknowledged by the controller: `playlist=all`, automatic cycling enabled, duration 30 seconds, 600 pixels, brightness 100%, power on. Order: Throb, Orange/Purple, Meteor Rain, Haunted Tide, Witchfire Sparkles. Meteor interval remains 1,250 ms. This changes live settings only; the existing firmware resets runtime selections to the two-animation preview after restart.

#  Bastion Honeypot (Fake SSH) — C++

Petit honeypot C++ conçu pour tourner sur un bastion exposé sur Internet.  
Il simule un service SSH-like (banner OpenSSH + login/password + shell fake), enregistre les connexions/commandes, et envoie des alertes stylées sur Discord via webhook.

---

## Features

- Serveur TCP async (Boost.Asio)
- Simulation SSH (banner + login/password)
- Fake shell interactif (commandes basiques)
- Logs locaux (connexions + commandes)
- Alertes Discord en temps réel (embeds + gravité)

---

## Requirements

- C++20
- CMake
- Boost (Asio)
- libcurl (webhook Discord)
- (j'ai eu la flemme d'utiliser vcpkg, donc vous devez installer les dépendances pour que ça fonctionne)
---

## Build

```bash
mkdir build
cd build
cmake ..
make
```
---

## ▶ Run

### Sans Discord

```bash
./service_honeypot
```

### Avec Discord alerts

```bash
DISCORD_WEBHOOK_URL="https://discord.com/api/webhooks/XXXX/XXXX" ./service_honeypot
```

⚠️ Ne jamais hardcoder le webhook dans le code, si tu push ton webhook je te spam de requêtes.

---

## Test / Usage

Connexion avec netcat :

```bash
nc localhost 69
```

Tu verras un banner SSH fake + login/password, puis un prompt shell.

Exemples de commandes :

```bash
ls
pwd
cat /etc/passwd
sudo -l
exit
```

---

## Logs

Les logs sont écrits dans :

- `logs/connections.log`
- `logs/commands.log`

⚠️ Avec CLion, le working directory est souvent `cmake-build-debug/`, donc les logs peuvent se retrouver dans :

```txt
cmake-build-debug/logs/
```

---

## Discord Alerts

Le honeypot peut envoyer automatiquement des embeds Discord lors :

- d’une connexion
- d’une commande exécutée

Certaines commandes déclenchent une gravité `ALERT` (ex: `sudo`, `nmap`, `wget`, `curl`, `/etc/shadow`).

---

## Project Structure

```txt
include/   -> headers
src/       -> code
logs/      -> logs runtime
```

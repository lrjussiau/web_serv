#  🌐 Webserv 🌐

## Introduction

Ce projet implémente un serveur web simplifié en C++ capable de gérer des connexions clients, de traiter des requêtes HTTP et de générer des réponses HTTP appropriées. Le projet est structuré en plusieurs classes, chacune ayant des responsabilités spécifiques.

## Structure des Classes

### Classe `Client`

#### Responsabilités
- Gérer les connexions individuelles des clients.
- Stocker et traiter les informations des requêtes HTTP des clients.

#### Fonctionnalités
- **Constructeurs et Destructeur** : Initialisent les objets `Client`, attribuent les sockets serveur et client, et gèrent les ID et noms de session.
- **Méthodes de traitement** :
  - `setData` : Lit et analyse les fichiers de requête HTTP, extrait les informations clés (méthode, URL, protocole, etc.).
  - `parsePostRequest` : Gère les requêtes POST, en traitant les corps de requête encodés par morceaux (chunked) ou non.
  - `parseChunkedBody` et `parseBody` : Traitent les différents types de corps de requête, écrivent les données dans des fichiers.
  - `getPathToUpload` : Détermine le chemin du fichier à uploader à partir des en-têtes de la requête.
- **Getters et Setters** : Permettent d'accéder et de modifier les attributs de la classe.  
---
### Structure `Location` et `ServerConfig`

#### Responsabilités
- Représenter la configuration d'un emplacement spécifique (Location) et du serveur dans son ensemble (ServerConfig).

#### Fonctionnalités
- **`Location`** : Décrit un emplacement spécifique sur le serveur, avec des attributs tels que le chemin, les méthodes autorisées, le dossier de racine, les scripts CGI, etc.
- **`ServerConfig`** : Décrit la configuration générale du serveur, y compris les ports d'écoute, le nom du serveur, les erreurs personnalisées, les tailles maximales de corps de requête, et les emplacements.
---
### Classe `Config`

#### Responsabilités
- Charger et analyser le fichier de configuration du serveur.

#### Fonctionnalités
- **Méthodes de parsing** :
  - `parseServerBlock` et `parseLocationBlock` : Lisent et analysent les blocs de configuration pour le serveur et les emplacements.
  - `parseConfigFile` : Point d'entrée pour charger et analyser le fichier de configuration.
- **Vérifications** :
  - `checkConfig` : Valide la configuration.
  - `checkIpv4` : Vérifie la validité de l'adresse IPv4.
---
### Classe `Response`

#### Responsabilités
- Générer des réponses HTTP basées sur les requêtes des clients et la configuration du serveur.

#### Fonctionnalités
- **Construction de la réponse** :
  - `buildResponse` : Construit la réponse HTTP complète.
  - `createContent` : Génère le contenu de la réponse.
  - `buildRedirectResponse` : Gère les réponses de redirection.
  - `init_headers` et `createStatusLine` : Initialisent les en-têtes et la ligne de statut de la réponse.
- **Gestion des requêtes spécifiques** :
  - `handleDirectory` et `generateAutoIndex` : Gèrent les requêtes de type répertoire.
  - `generateCgi` : Exécute les scripts CGI.
- **Vérifications** :
  - `isMethodWrong`, `isCookie`, `isCGI`, `checkMimeType` : Vérifient différents aspects de la requête pour déterminer le traitement approprié.
---
### Classe `Server`

#### Responsabilités
- Créer et gérer les sockets du serveur.
- Gérer les connexions entrantes.

#### Fonctionnalités
- **Création du serveur** :
  - `createServer` : Initialise les sockets du serveur en fonction de la configuration.
  - `launchSocket` : Lance un socket sur un port et une adresse IP spécifiés.
---
### Classe `Supervisor`

#### Responsabilités
- Supervise les opérations du serveur, y compris la gestion des connexions clients et des serveurs.

#### Fonctionnalités
- **Gestion des connexions** :
  - `acceptNewConnection` : Accepte les nouvelles connexions clients.
  - `closeClient` et `closeServer` : Ferment les connexions clients et serveurs respectivement.
- **Gestion des opérations** :
  - `manageOperations` : Supervise les opérations de lecture et d'écriture sur les sockets.
  - `readRequestFromClient` et `writeResponseToClient` : Lisent les requêtes des clients et écrivent les réponses.
- **Mises à jour des descripteurs de fichiers** :
  - `updateFdMax` et `fdSetRemove` : Gèrent les descripteurs de fichiers pour `select`.

---	
# Conclusion

Le projet WebServ implémente un serveur web en C++ capable de :
- Charger et analyser sa configuration.
- Accepter et gérer les connexions clients.
- Traiter les requêtes HTTP GET et DELETE.
- Traiter les requêtes HTTP POST avec ou sans encodage par morceaux.
- Générer des réponses HTTP appropriées.
- Superviser les opérations de lecture et d'écriture sur les sockets.

Chaque classe a une responsabilité spécifique, ce qui permet une modularité et une maintenabilité accrues.